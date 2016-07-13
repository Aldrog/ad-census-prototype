#ifndef PRINTER_VISITOR_OLD_H_
#define PRINTER_VISITOR_OLD_H_

#include <stdint.h>
#include <iostream>
#include <sstream>

#include "reflection.h"

namespace corecvs {

using std::string;
using std::ostream;
using std::cout;

class PrinterVisitor
{
public:
    bool isSaver () { return false;}
    bool isLoader() { return false;}


public:
    std::ostream *stream;
    int indentation;
    int dIndent;

    explicit  PrinterVisitor(ostream *_stream) :
        stream(_stream),
        indentation(0),
        dIndent(1)
    {}

    explicit  PrinterVisitor(ostream &_stream = cout) :
        stream(&_stream),
        indentation(0),
        dIndent(1)
    {}

    explicit  PrinterVisitor(int indent, int dindent, ostream &_stream = cout) :
        stream(&_stream),
        indentation(indent),
        dIndent(dindent)
    {}

    std::string indent() {
        return std::string(indentation, ' ');
    }

/* Generic Array support */
    template <typename inputType, typename reflectionType>
    void visit(std::vector<inputType> &fields, const reflectionType * /*fieldDescriptor*/)
    {
        indentation += dIndent;
        for (int i = 0; i < fields.size(); i++)
        {
            fields[i].accept(*this);
        }
        indentation -= dIndent;
    }

    template <typename innerType>
    void visit(std::vector<innerType> &field, const char* arrayName)
    {
        indentation += dIndent;
        for (size_t i = 0; i < field.size(); i++)
        {
            std::ostringstream ss;
            ss << arrayName << "[" <<  i << "]";
            visit<innerType>(field[i], innerType(), ss.str().c_str());
        }
        indentation -= dIndent;
    }

template <typename inputType, typename reflectionType>
    void visit(inputType &field, const reflectionType * fieldDescriptor)
    {
        if (stream != NULL) {
            *stream << indent() << fieldDescriptor->getSimpleName() << ":" << std::endl;
        }
        indentation += dIndent;
        field.accept(*this);
        indentation -= dIndent;
    }

template <class Type>
    void visit(Type &field, const char * fieldName)
    {
        if (stream != NULL) {
            *stream << indent() << fieldName << ":" << std::endl;
        }
        indentation += dIndent;
        field.accept(*this);
        indentation -= dIndent;
    }

template <class Type>
    void visit(Type &field, Type /*defaultValue*/, const char * fieldName)
	{
        visit<Type>(field, fieldName);
	}

};


template <>
void PrinterVisitor::visit<int,    IntField>(int &field, const IntField *fieldDescriptor);

template <>
void PrinterVisitor::visit<double, DoubleField>(double &field, const DoubleField *fieldDescriptor);

template <>
void PrinterVisitor::visit<float,  FloatField>(float &field, const FloatField *fieldDescriptor);

template <>
void PrinterVisitor::visit<bool,   BoolField>(bool &field, const BoolField *fieldDescriptor);

template <>
void PrinterVisitor::visit<string, StringField>(string &field, const StringField *fieldDescriptor);

template <>
void PrinterVisitor::visit<void *, PointerField>(void * &field, const PointerField *fieldDescriptor);

template <>
void PrinterVisitor::visit<int,    EnumField>(int &field, const EnumField *fieldDescriptor);

template <>
void PrinterVisitor::visit<std::string, StringField>(std::string &field, const StringField *fieldDescriptor);

/* Arrays */

template <>
void PrinterVisitor::visit<double, DoubleVectorField>(std::vector<double> &field, const DoubleVectorField *fieldDescriptor);

/**
 * Old Style
 *
 * this methods can be made universal, but are separated to make it a bit more controllable
 **/

template <>
void PrinterVisitor::visit<int>(int &intField, int defaultValue, const char *fieldName);

template <>
void PrinterVisitor::visit<uint64_t>(uint64_t &intField, uint64_t defaultValue, const char *fieldName);

template <>
void PrinterVisitor::visit<double>(double &doubleField, double defaultValue, const char *fieldName);

template <>
void PrinterVisitor::visit<float>(float &floatField, float defaultValue, const char *fieldName);

template <>
void PrinterVisitor::visit<bool>(bool &boolField, bool defaultValue, const char *fieldName);

template <>
void PrinterVisitor::visit<std::string>(std::string &stringField, std::string defaultValue, const char *fieldName);


} //namespace corecvs
#endif // PRINTER_VISITOR_OLD_H_
