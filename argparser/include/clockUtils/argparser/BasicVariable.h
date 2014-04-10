#ifndef __CLOCKUTILS_ARGPARSER_BASICVARIABLE_H__
#define __CLOCKUTILS_ARGPARSER_BASICVARIABLE_H__

#include "clockUtils/argparser/argParserParameters.h"

#include <string>

namespace clockUtils {
namespace argparser {

	class CLOCK_ARGPARSER_API BasicVariable {
	public:
		BasicVariable(const std::string & name, const std::string & description, const std::string & type);

		std::string getName() const {
			return _name;
		}

		std::string getType() const {
			return _type;
		}

		virtual bool setValue(const std::string & value) = 0;

	private:
		std::string _name;
		std::string _description;
		std::string _type;
	};

	template<typename T>
	class Variable : public BasicVariable {
	public:
		Variable(const std::string & name, const std::string & description, T * reference, const std::string & type) : BasicVariable(name, description, type), _reference(reference) {
		}

		bool setValue(const std::string & value) {
			std::stringstream ss(value);
			return !(ss >> (*_reference)).fail();
		}

	private:
		T * _reference;
	};

} /* namespace argparser */
} /* namespace clockUtils */

#endif /* __CLOCKUTILS_ARGPARSER_BASICVARIABLE_H__ */
