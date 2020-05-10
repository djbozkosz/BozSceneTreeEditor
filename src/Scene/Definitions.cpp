#include <QFile>
#include <QStringList>

#include "Scene/Definitions.h"


using namespace Djbozkosz::Application::Scene;


const QVector<Definitions::NodeFieldType> Definitions::FIELD_TYPES =
{
	{ "Unknown", Definitions::ENodeFieldType::Unknown, 0                 },
	{ "Uint8",   Definitions::ENodeFieldType::Uint8,   sizeof(uchar)     },
	{ "Uint16",  Definitions::ENodeFieldType::Uint16,  sizeof(ushort)    },
	{ "Uint32",  Definitions::ENodeFieldType::Uint32,  sizeof(uint)      },
	{ "Int8",    Definitions::ENodeFieldType::Int8,    sizeof(char)      },
	{ "Int16",   Definitions::ENodeFieldType::Int16,   sizeof(short)     },
	{ "Int32",   Definitions::ENodeFieldType::Int32,   sizeof(int)       },
	{ "Hex8",    Definitions::ENodeFieldType::Hex8,    sizeof(uchar)     },
	{ "Hex16",   Definitions::ENodeFieldType::Hex16,   sizeof(ushort)    },
	{ "Hex32",   Definitions::ENodeFieldType::Hex32,   sizeof(uint)      },
	{ "Float",   Definitions::ENodeFieldType::Float,   sizeof(float)     },
	{ "Float2",  Definitions::ENodeFieldType::Float2,  sizeof(float) * 2 },
	{ "Float3",  Definitions::ENodeFieldType::Float3,  sizeof(float) * 3 },
	{ "Float4",  Definitions::ENodeFieldType::Float4,  sizeof(float) * 4 },
	{ "Color",   Definitions::ENodeFieldType::Color,   sizeof(float) * 3 },
	{ "String",  Definitions::ENodeFieldType::String,  0                 }
};


Definitions::Definitions() :
	m_RNodeLine("^Node ([a-fx0-9]+)_?([a-fx0-9]*) +([a-zA-Z0-9_]+) +([a-zA-Z0-9_]+)? *\\[(.*)\\]$"),
	m_RNodeFieldList(", *"),
	m_RNodeField(" *\\: *"),
	m_RNodeNameLine("^NodeName ([a-fx0-9]+)_?([a-fx0-9]*) +\\[([0-9]+)\\]$"),
	m_RNodeFieldEnum("^Enum ([a-fx0-9]+) \\[([0-9]+)\\] +\\[(.*)\\]$")
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

const Definitions::NodeDefinition* Definitions::GetNodeDefinition(ushort parentType, ushort type) const
{
	auto key        = GetKey(parentType, type);
	auto definition = m_NodeTypeToDefinition.find(key);

	if (definition == m_NodeTypeToDefinition.end())
		return null;

	return &definition.value();
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
			LoadDefinition(*line);
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

void Definitions::LoadDefinition(const QString& line)
{
	auto match = m_RNodeLine.exactMatch(line);
	Debug::Assert(match == true) << "Invalid definition:" << line;
	Debug::Assert(m_RNodeLine.captureCount() == 5) << "Invalid parameters count for definition:" << line;

	auto captures = m_RNodeLine.capturedTexts();

	NodeDefinition definition;
	auto parentType = captures[2].isEmpty() ? (short)0                      : captures[1].toUShort(null, 0);
	auto type       = captures[2].isEmpty() ? captures[1].toUShort(null, 0) : captures[2].toUShort(null, 0);
	auto key        = GetKey(parentType, type);

	definition.Type      = type;
	definition.Name      = captures[3];
	definition.HasChilds = (captures[4] == "has_childs");

	Debug::Assert(m_NodeTypeToDefinition.contains(key) == false) << "Node definitions already contain type" << type;

	auto fieldList = captures[5].split(m_RNodeFieldList, QString::SkipEmptyParts);
	definition.Fields.reserve(fieldList.size());

	foreach (field, fieldList)
	{
		auto values = field->split(m_RNodeField, QString::SkipEmptyParts);
		Debug::Assert(values.size() == 2) << "Invalid field definition:" << *field;

		auto fieldType = m_StringToField.find(values[0]);
		Debug::Assert(fieldType != m_StringToField.end()) << "Invalid field type:" << values[0];

		NodeFieldInfo fieldInfo;
		fieldInfo.FieldType = fieldType.value();
		fieldInfo.Name      = values[1];

		definition.Fields.push_back(fieldInfo);
	}

	m_NodeTypeToDefinition[key] = definition;
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
	auto match = m_RNodeFieldEnum.exactMatch(line);
	Debug::Assert(match == true) << "Invalid node field enum:" << line;
	Debug::Assert(m_RNodeFieldEnum.captureCount() == 3) << "Invalid parameters count for node field enum:" << line;

	auto captures = m_RNodeFieldEnum.capturedTexts();

	auto type      = captures[1].toUShort(null, 0);
	auto fieldIdx  = captures[2].toUShort(null, 0);
	auto fieldList = captures[3].split(m_RNodeFieldList, QString::SkipEmptyParts);

	NodeFieldEnum enumMap;

	foreach (field, fieldList)
	{
		auto values = field->split(m_RNodeField, QString::SkipEmptyParts);
		Debug::Assert(values.size() == 2) << "Invalid field definition:" << *field;

		auto key     = values[0].toUShort(null, 0);
		enumMap[key] = values[1];
	}

	auto& enums     = m_NodeFieldEnums[type];
	enums[fieldIdx] = enumMap;
}
