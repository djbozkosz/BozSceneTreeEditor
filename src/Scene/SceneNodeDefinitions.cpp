#include <QFile>
#include <QStringList>

#include "Scene/SceneNodeDefinitions.h"


using namespace Djbozkosz::Application::Scene;


const QVector<SceneNodeDefinitions::NodeFieldType> SceneNodeDefinitions::FIELD_TYPES =
{
	{ "Uint8",  SceneNodeDefinitions::ENodeFieldType::Uint8,  sizeof(uchar)     },
	{ "Uint16", SceneNodeDefinitions::ENodeFieldType::Uint16, sizeof(ushort)    },
	{ "Uint32", SceneNodeDefinitions::ENodeFieldType::Uint32, sizeof(uint)      },
	{ "Int8",   SceneNodeDefinitions::ENodeFieldType::Int8,   sizeof(char)      },
	{ "Int16",  SceneNodeDefinitions::ENodeFieldType::Int16,  sizeof(short)     },
	{ "Int32",  SceneNodeDefinitions::ENodeFieldType::Int32,  sizeof(int)       },
	{ "Hex8",   SceneNodeDefinitions::ENodeFieldType::Hex8,   sizeof(uchar)     },
	{ "Hex16",  SceneNodeDefinitions::ENodeFieldType::Hex16,  sizeof(ushort)    },
	{ "Hex32",  SceneNodeDefinitions::ENodeFieldType::Hex32,  sizeof(uint)      },
	{ "Float",  SceneNodeDefinitions::ENodeFieldType::Float,  sizeof(float)     },
	{ "Float2", SceneNodeDefinitions::ENodeFieldType::Float2, sizeof(float) * 2 },
	{ "Float3", SceneNodeDefinitions::ENodeFieldType::Float3, sizeof(float) * 3 },
	{ "Float4", SceneNodeDefinitions::ENodeFieldType::Float4, sizeof(float) * 4 },
	{ "Color",  SceneNodeDefinitions::ENodeFieldType::Color,  sizeof(float) * 3 },
	{ "String", SceneNodeDefinitions::ENodeFieldType::String, 0                 }
};


SceneNodeDefinitions::SceneNodeDefinitions() :
	m_RegExtSplitLine("^([a-fx0-9]+) +([a-zA-Z0-9_]+) +([a-zA-Z0-9_]+)? *\\[(.*)\\]$"),
	m_RegExtSplitFieldList(", *"),
	m_RegExtSplitField(" *\\: *")
{
	foreach (field, FIELD_TYPES)
	{
		m_StringToField[field->Name] = &(*field);
	}

	Load("node_definitions.txt");
}

SceneNodeDefinitions::~SceneNodeDefinitions()
{
}

const SceneNodeDefinitions::NodeDefinition* SceneNodeDefinitions::GetDefinition(ushort type) const
{
	auto definition = m_NodeTypeToDefinition.find(type);
	if (definition == m_NodeTypeToDefinition.end())
		return null;

	return &definition.value();
}

void SceneNodeDefinitions::Load(const QString &file)
{
	QFile reader(file);
	auto result = reader.open(QIODevice::ReadOnly);
	if (result == false)
		return; // assert

	auto lines = QString::fromLatin1(reader.readAll()).split("\r\n", QString::SkipEmptyParts);
	foreach (line, lines)
	{
		auto matched = m_RegExtSplitLine.exactMatch(*line);
		if (matched == false)
			continue; // assert

		if (m_RegExtSplitLine.captureCount() < 4)
			continue; // assert

		auto captures = m_RegExtSplitLine.capturedTexts();

		NodeDefinition definition;
		definition.Type      = captures[1].toUShort(null, 0);
		definition.Name      = captures[2];
		definition.HasChilds = (captures[3] == "has_childs");

		auto fieldList = captures[4].split(m_RegExtSplitFieldList, QString::SkipEmptyParts);
		definition.Fields.reserve(fieldList.size());

		foreach (field, fieldList)
		{
			auto values = field->split(m_RegExtSplitField, QString::SkipEmptyParts);
			if (values.size() < 2)
				continue; // assert

			auto fieldType = m_StringToField.find(values[0]);
			if (fieldType == m_StringToField.end())
				continue; // assert

			NodeFieldInfo fieldInfo;
			fieldInfo.FieldType = fieldType.value();
			fieldInfo.Name      = values[1];

			definition.Fields.push_back(fieldInfo);
		}

		m_NodeTypeToDefinition[definition.Type] = definition;
	}
}
