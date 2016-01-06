compiler=('g++' 'clang++')
buildType=("Debug" "Release")
# testing needs to be the first (so test are run correctly)
configs=('TESTING')
# what tests exist and how often to run them
tests=('./ArgumentParserTester' './CompressionTester' './SocketsTester' './iniParserTester')
testcount=(1 1 5 1)

function build() {
	id=$1
	cl=$2
	runTests=$3

	# prepare
	rm -rf BuildAll_$id
	mkdir BuildAll_$id
	cd BuildAll_$id

	# build
	cmd="cmake $cl"
	eval $cmd
	if [ $? -ne 0 ]; then
		return
	fi
	make
	if [ "$runTests" == true ]; then
		echo "Running Tests"
		cd bin
		testId=0
		while [ $testId -lt $((${#tests[@]})) ]; do
			testRun=0
			count=${testcount[testId]}
			while [ $testRun -lt $count ]; do # ${testcount[$testId]} ]; do
				eval ${tests[$testId]}
				let testRun+=1
			done
			let testId+=1
		done
		cd ..
	fi

	# clean
	cd ..
}

function run() {
	bt=$1
	comp=$2
	config=$3
	str="-DCMAKE_BUILD_TYPE=$bt"
	c=0
	while [ $c -lt $((${#configs[@]})) ]; do
		if [ $(($config & (2**$c))) -ne 0 ]; then
			str="$str -DWITH_${configs[$c]}=ON"
		else
			str="$str -DWITH_${configs[$c]}=OFF"
		fi
		let c+=1
	done
	if [ $((config%2)) -eq 0 ]; then
		runTests=false
	else
		runTests=true
	fi
	echo "Building with compiler '$comp' with num '$config': $str ..."
	commandLine="-DCMAKE_CXX_COMPILER=$comp $str $sourceDir"
	build ${bt}_${comp}_${config} "$commandLine" $runTests #&> build_${bt}_${comp}_${config}.log
}

if [ $# -lt 2 ]; then
	echo -e "Usage:\n\tbuildAll.sh <sourceDir> <buildDir> [<numStart>]"
	exit 1
fi

sourceDir=`realpath $1`
buildDir=`realpath $2`

cd $buildDir

for bt in ${buildType[@]}
do
	for comp in ${compiler[@]}
	do
		config=0
		if ! [ -z "$3" ]; then
			config=$3
		fi
		while [ $config -lt $((2**${#configs[@]})) ]; do
			run $bt $comp $config
			let config+=1
		done
	done
done
