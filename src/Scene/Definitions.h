#ifndef APPLICATION_SCENE_DEFINITIONS_H
#define APPLICATION_SCENE_DEFINITIONS_H

#include <QString>
#include <QRegExp>
#include <QVector>
#include <QMap>

#include "Utility/Base.h"


namespace Djbozkosz {
namespace Application {
namespace Scene
{
	sealed class Definitions
	{
		public: // types

		enum class ENodeFieldType
		{
			Unknown = 0,
			Uint8,
			Uint16,
			Uint32,
			Int8,
			Int16,
			Int32,
			Hex8,
			Hex16,
			Hex32,
			Float,
			Float2,
			Float3,
			Float4,
			Color,
			String,
			StringArray,
			StringFixed
		};

		sealed struct NodeFieldType
		{
			ENodeFieldType Type;
			uint           Size;
			QString        Name;

			inline NodeFieldType(const QString& name, ENodeFieldType type, uint size)
				: Type(type), Size(size), Name(name)
			{
			}
		};

		sealed struct NodeFieldInfo
		{
			const NodeFieldType* FieldType;
			QString              Name;
			uint                 FixedSize;

			inline NodeFieldInfo() : FieldType(0), FixedSize(0) {}
		};

		sealed struct NodeDefinition
		{
			ushort                 Type;
			QString                Name;
			bool                   HasChilds;
			QVector<NodeFieldInfo> Fields;

			inline NodeDefinition() : Type(0), HasChilds(false) {}
		};

		sealed struct NodeFieldDefinition
		{
			ushort                     SiblingType;
			uint                       SiblingFieldIdx;
			QMap<uint, NodeDefinition> Definitions;

			inline NodeFieldDefinition() : SiblingType(0), SiblingFieldIdx(0) {}
		};

		sealed struct NodeName
		{
			ushort Type;
			ushort ChildType;
			uint   FieldIdx;

			inline NodeName() : Type(0), ChildType(0), FieldIdx(0) {}
		};

		private: // constants

		static const QVector<NodeFieldType> FIELD_TYPES;

		private: // members

		QRegExp                             m_RNodeLine;
		QRegExp                             m_RNodeFieldList;
		QRegExp                             m_RNodeField;
		QRegExp                             m_RNodeFieldLine;
		QRegExp                             m_RNodeNameLine;
		QRegExp                             m_RNodeFieldEnumLine;

		QMap<QString, const NodeFieldType*> m_StringToField;
		QMap<uint, NodeDefinition>          m_NodeDefinitions;

		QMap<ushort, NodeFieldDefinition>   m_NodeFieldDefinitions;

		QMap<ushort, NodeName>              m_NodeNames;

		typedef QMap<int, QString>                       NodeFieldEnum;
		typedef QMap<ushort, QMap<uint, NodeFieldEnum> > NodeFieldEnums;
		NodeFieldEnums                      m_NodeFieldEnums;

		public: // methods

		explicit Definitions();
		virtual ~Definitions();

		const NodeDefinition*      GetNodeDefinition(ushort parentType, ushort type) const;
		const NodeFieldDefinition* GetNodeFieldDefinition(ushort type) const;
		const NodeDefinition*      GetNodeFieldDefinitionData(ushort type, uint dataType) const;
		const NodeName*            GetNodeName(ushort type) const;
		const NodeFieldEnum*       GetNodeFieldEnum(ushort type, uint fieldIdx) const;

		private: // methods

		void Load(const QString& file);
		void LoadDefinition(const QString& line);
		void LoadNodeFields(const QString& line);
		void LoadFields(NodeDefinition& definition, const QString& fields);
		void LoadField(const QString& field, QString& type, QString& name, uint& fixedSize);
		void LoadNodeName(const QString& line);
		void LoadNodeFieldEnum(const QString& line);

		static inline uint GetKey(ushort parentType, ushort type) { return (parentType << 16) | type; }
	};
}}}

#endif // APPLICATION_SCENE_DEFINITIONS_H
