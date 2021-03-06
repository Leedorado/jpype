/*****************************************************************************
   Copyright 2004 Steve M�nard

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
   
*****************************************************************************/   
#include <jpype.h>

JPField::JPField() :
    m_Name(""),
    m_Class(NULL),
    m_IsStatic(false),
    m_IsFinal(false),
    m_Field(),
    m_FieldID(NULL),
    m_Type()
{
}

JPField::JPField(JPClass* clazz, jobject fld)
{
	TRACE_IN("JPField::JPField1");
	
	m_Class = clazz;
	m_Field = JPEnv::getJava()->NewGlobalRef(fld);
	
	m_Name = JPJni::getMemberName(fld);
	
	m_IsStatic = JPJni::isMemberStatic(fld);
	m_IsFinal = JPJni::isMemberFinal(fld);
	m_FieldID = JPEnv::getJava()->FromReflectedField(fld);
	m_Type = JPJni::getType(m_Field);	

	TRACE2("field type", m_Type.getSimpleName());
	

	TRACE_OUT
}

JPField::JPField(const JPField& fld)
{
	TRACE_IN("JPField::JPField2");
	
	m_Name = fld.m_Name; 
	m_IsStatic = fld.m_IsStatic;
	m_IsFinal = fld.m_IsFinal;
	m_FieldID = fld.m_FieldID;
	m_Type = fld.m_Type;

	m_Class = fld.m_Class;

	m_Field = JPEnv::getJava()->NewGlobalRef(fld.m_Field);
	TRACE_OUT;
}

JPField::~JPField() NO_EXCEPT_FALSE
{
	TRACE_IN("JPField::~JPField");
	JPEnv::getJava()->DeleteGlobalRef(m_Field);
	TRACE_OUT;
}
	
bool JPField::isStatic() const
{
	return m_IsStatic;
}
	
const string& JPField::getName() const
{
	return m_Name;
}	

HostRef* JPField::getStaticAttribute() 
{
	TRACE_IN("JPField::getStaticAttribute");
	JPType* type = JPTypeManager::getType(m_Type);
	jclass claz = m_Class->getClass();
	return type->getStaticValue(claz, m_FieldID, m_Type);
	TRACE_OUT;	
}

void JPField::setStaticAttribute(HostRef* val) 
{
	TRACE_IN("JPField::setStaticAttribute");

	if (m_IsFinal)
	{
		stringstream err;
		err << "Field " << m_Name << " is read-only";
		RAISE(JPypeException, err.str().c_str());
	}

	JPType* type = JPTypeManager::getType(m_Type);
	if (type->canConvertToJava(val) <= _explicit)
	{
		stringstream err;
		err << "unable to convert to " << type->getName().getSimpleName();
		JPEnv::getHost()->setTypeError( err.str().c_str());
	}
		
	jclass claz = m_Class->getClass();
	type->setStaticValue(claz, m_FieldID, val);		
	TRACE_OUT;
}

HostRef* JPField::getAttribute(jobject inst) 
{
	TRACE_IN("JPField::getAttribute");
	TRACE2("field type", m_Type.getSimpleName()); 
	JPType* type = JPTypeManager::getType(m_Type);
	
	return type->getInstanceValue(inst, m_FieldID, m_Type);	
	TRACE_OUT;
}

void JPField::setAttribute(jobject inst, HostRef* val) 
{
	TRACE_IN("JPField::setAttribute");
	if (m_IsFinal)
	{
		stringstream err;
		err << "Field " << m_Name << " is read-only";
		RAISE(JPypeException, err.str().c_str());
	}

	JPType* type = JPTypeManager::getType(m_Type);
	if (type->canConvertToJava(val) <= _explicit)
	{
		stringstream err;
		err << "unable to convert to " << type->getName().getSimpleName();
		JPEnv::getHost()->setTypeError( err.str().c_str());
	}
		
	type->setInstanceValue(inst, m_FieldID, val);		
	TRACE_OUT;
}

