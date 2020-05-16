#include <QFile>
#include <QStringList>

#include "Scene/Definitions.h"


using namespace Djbozkosz::Application::Scene;


const QVector<Definitions::NodeFieldType> Definitions::FIELD_TYPES =
{
	{ "Unknown",     Definitions::ENodeFieldType::Unknown,     0                 },
	{ "Uint8",       Definitions::ENodeFieldType::Uint8,       sizeof(uchar)     },
	{ "Uint16",      Definitions::ENodeFieldType::Uint16,      sizeof(ushort)    },
	{ "Uint32",      Definitions::ENodeFieldType::Uint32,      sizeof(uint)      },
	{ "Int8",        Definitions::ENodeFieldType::Int8,        sizeof(char)      },
	{ "Int16",       Definitions::ENodeFieldType::Int16,       sizeof(short)     },
	{ "Int32",       Definitions::ENodeFieldType::Int32,       sizeof(int)       },
	{ "Hex8",        Definitions::ENodeFieldType::Hex8,        sizeof(uchar)     },
	{ "Hex16",       Definitions::ENodeFieldType::Hex16,       sizeof(ushort)    },
	{ "Hex32",       Definitions::ENodeFieldType::Hex32,       sizeof(uint)      },
	{ "Float",       Definitions::ENodeFieldType::Float,       sizeof(float)     },
	{ "Float2",      Definitions::ENodeFieldType::Float2,      sizeof(float) * 2 },
	{ "Float3",      Definitions::ENodeFieldType::Float3,      sizeof(float) * 3 },
	{ "Float4",      Definitions::ENodeFieldType::Float4,      sizeof(float) * 4 },
	{ "Color",       Definitions::ENodeFieldType::Color,       sizeof(float) * 3 },
	{ "String",      Definitions::ENodeFieldType::String,      1                 },
	{ "StringArray", Definitions::ENodeFieldType::StringArray, sizeof(uint)      },
	{ "StringFixed", Definitions::ENodeFieldType::StringFixed, 0                 },
	{ "Struct",      Definitions::ENodeFieldType::Struct,      0                 }
};


Definitions::Definitions() :
	m_RNodeLine("^Node\\s+([a-fx0-9]+)_?([a-fx0-9]*)\\s+([a-zA-Z0-9_]+)\\s+([a-zA-Z0-9_]+)?\\s*\\[(.*)\\]$"),
	m_RStructLine("^Struct\\s+([a-zA-Z0-9_]+)\\s+\\[(.*)\\]$"),
	m_RNodeFieldList(",\\s*"),
	m_RNodeField("^([a-zA-Z0-9_]+)\\'?([0-9]*)\\s*\\:\\s*([a-zA-Z0-9_]+)$"),
	m_RNodeFieldLine("^NodeFields\\s+([a-fx0-9]+)\\s+([a-fx0-9]+)\\s+\\[([0-9]+)\\]\\s+([0-9]+)\\s+\\[(.*)\\]$"),
	m_RNodeNameLine("^NodeName\\s+([a-fx0-9]+)_?([a-fx0-9]*)\\s*\\[([0-9]+)\\]$"),
	m_RNodeFieldEnumLine("^Enum\\s+([a-fx0-9]+)\\s+\\[([0-9]+)\\]\\s+\\[(.*)\\]$")
{
	foreach (field, FIELD_TYPES)
	{
		m_StringToField[field->Name] = &(*field);
	}

	Load("definitions.txt");
}

Definitions::~Definitions()
{
}

const Definitions::NodeDefinition* Definitions::GetNode(ushort parentType, ushort type) const
{
	auto key        = GetKey(parentType, type);
	auto definition = m_Nodes.find(key);

	if (definition == m_Nodes.end())
		return null;

	return &definition.value();
}

const Definitions::NodeFieldDefinition* Definitions::GetNodeField(ushort type) const
{
	auto fieldDefinitions = m_NodeFields.find(type);
	if (fieldDefinitions == m_NodeFields.end())
		return null;

	return &fieldDefinitions.value();
}

const Definitions::NodeDefinition* Definitions::GetNodeFieldData(ushort type, uint dataType) const
{
	auto fieldDefinitions = GetNodeField(type);
	if (fieldDefinitions == null)
		return null;

	auto fieldDefinition = fieldDefinitions->Definitions.find(dataType);
	if (fieldDefinition == fieldDefinitions->Definitions.end())
		return null;

	return &fieldDefinition.value();
}

const Definitions::NodeName* Definitions::GetNodeName(ushort type) const
{
	auto name = m_NodeNames.find(type);

	if (name == m_NodeNames.end())
		return null;

	return &name.value();
}

const Definitions::NodeFieldEnum* Definitions::GetNodeFieldEnum(ushort type, uint fieldIdx) const
{
	auto fields = m_NodeFieldEnums.find(type);
	if (fields == m_NodeFieldEnums.end())
		return null;

	auto enumMap = fields->find(fieldIdx);
	if (enumMap == fields->end())
		return null;

	return &enumMap.value();
}

void Definitions::Load(const QString &file)
{
	QFile reader(file);
	auto result = reader.open(QIODevice::ReadOnly);
	Debug::Assert(result == true) << "Cannot open definition file" << file;

	auto lines = QString::fromLatin1(reader.readAll()).split("\r\n", QString::SkipEmptyParts);
	foreach (line, lines)
	{
		if (line->startsWith("Node ") == true)
		{
			LoadNode(*line);
		}
		else if (line->startsWith("Struct ") == true)
		{
			LoadStruct(*line);
		}
		else if (line->startsWith("NodeFields ") == true)
		{
			LoadNodeFields(*line);
		}
		else if (line->startsWith("NodeName ") == true)
		{
			LoadNodeName(*line);
		}
		else if (line->startsWith("Enum ") == true)
		{
			LoadNodeFieldEnum(*line);
		}
	}
}

void Definitions::LoadNode(const QString& line)
{
	auto match = m_RNodeLine.exactMatch(line);
	Debug::Assert(match == true) << "Invalid node:" << line;
	Debug::Assert(m_RNodeLine.captureCount() == 5) << "Invalid parameters count for node:" << line;

	auto captures = m_RNodeLine.capturedTexts();

	NodeDefinition node;
	auto parentType = captures[2].isEmpty() ? (short)0                      : captures[1].toUShort(null, 0);
	auto type       = captures[2].isEmpty() ? captures[1].toUShort(null, 0) : captures[2].toUShort(null, 0);
	auto key        = GetKey(parentType, type);

	node.Type       = type;
	node.Name       = captures[3];
	node.HasChilds  = (captures[4] == "has_childs");

	Debug::Assert(m_Nodes.contains(key) == false) << "Node already contain type" << type;

	LoadFields(node.Fields, captures[5]);
	m_Nodes[key] = node;
}

void Definitions::LoadStruct(const QString& line)
{
	auto match = m_RStructLine.exactMatch(line);
	Debug::Assert(match == true) << "Invalid struct:" << line;
	Debug::Assert(m_RStructLine.captureCount() == 2) << "Invalid parameters count for struct:" << line;

	auto captures = m_RStructLine.capturedTexts();

	auto name = captures[1];
	Debug::Assert(m_Structs.contains(name) == false) << "Structs already contain type" << name;

	StructDefinition strukt;
	strukt.Name = name;

	LoadFields(strukt.Fields, captures[2]);
	m_Structs[name] = strukt;
}

void Definitions::LoadNodeFields(const QString& line)
{
	auto match = m_RNodeFieldLine.exactMatch(line);
	Debug::Assert(match == true) << "Invalid node field definition:" << line;
	Debug::Assert(m_RNodeFieldLine.captureCount() == 5) << "Invalid parameters count for node field definition:" << line;

	auto captures         = m_RNodeFieldLine.capturedTexts();

	auto  type            = captures[1].toUShort(null, 0);
	auto& field           = m_NodeFields[type];
	auto  dataType        = captures[4].toUInt(null, 0);
	field.SiblingType     = captures[2].toUShort(null, 0);
	field.SiblingFieldIdx = captures[3].toUInt(null, 0);

	auto node             = *GetNode(0, type);
	node.Fields.clear();
	LoadFields(node.Fields, captures[5]);
	field.Definitions[dataType] = node;
}

void Definitions::LoadFields(QVector<NodeFieldInfo>& fieldInfos, const QString& fields)
{
	auto fieldList = fields.split(m_RNodeFieldList, QString::SkipEmptyParts);
	fieldInfos.reserve(fieldList.size());

	foreach (field, fieldList)
	{
		QString type;
		QString name;
		uint    size;
		LoadField(*field, type, name, size);

		NodeFieldInfo fieldInfo;

		auto fieldType = m_StringToField.find(type);
		if (fieldType == m_StringToField.end())
		{
			fieldType   = m_StringToField.find("Struct");
			auto strukt = m_Structs.find(type);
			Debug::Assert(strukt != m_Structs.end()) << "Invalid field type:" << type;

			fieldInfo.NestedField = &strukt.value();
		}

		fieldInfo.FieldType = fieldType.value();
		fieldInfo.Name      = name;
		fieldInfo.FixedSize = size;

		fieldInfos.push_back(fieldInfo);
	}
}

void Definitions::LoadField(const QString& field, QString& type, QString& name, uint& size)
{
	auto match = m_RNodeField.exactMatch(field);
	Debug::Assert(match == true) << "Invalid field format:" << field;
	Debug::Assert(m_RNodeField.captureCount() == 3) << "Invalid parameters count for field:" << field;

	auto captures = m_RNodeField.capturedTexts();
	type          = captures[1];
	name          = captures[3];
	size          = captures[2].toUInt();
}

void Definitions::LoadNodeName(const QString& line)
{
	auto match = m_RNodeNameLine.exactMatch(line);
	Debug::Assert(match == true) << "Invalid node name:" << line;
	Debug::Assert(m_RNodeNameLine.captureCount() == 3) << "Invalid parameters count for node name:" << line;

	auto captures = m_RNodeNameLine.capturedTexts();

	NodeName nodeName;
	nodeName.Type      = captures[1].toUShort(null, 0);
	nodeName.ChildType = captures[2].isEmpty() ? (short)0 : captures[2].toUShort(null, 0);
	nodeName.FieldIdx  = captures[3].toUInt(null, 0);

	m_NodeNames[nodeName.Type] = nodeName;
}

void Definitions::LoadNodeFieldEnum(const QString& line)
{
	auto match = m_RNodeFieldEnumLine.exactMatch(line);
	Debug::Assert(match == true) << "Invalid node field enum:" << line;
	Debug::Assert(m_RNodeFieldEnumLine.captureCount() == 3) << "Invalid parameters count for node field enum:" << line;

	auto captures = m_RNodeFieldEnumLine.capturedTexts();

	auto type      = captures[1].toUShort(null, 0);
	auto fieldIdx  = captures[2].toUShort(null, 0);
	auto fieldList = captures[3].split(m_RNodeFieldList, QString::SkipEmptyParts);

	NodeFieldEnum enumMap;

	foreach (field, fieldList)
	{
		QString idx;
		QString name;
		uint    size;
		LoadField(*field, idx, name, size);

		auto key     = idx.toUShort(null, 0);
		enumMap[key] = name;
	}

	auto& enums     = m_NodeFieldEnums[type];
	enums[fieldIdx] = enumMap;
}
