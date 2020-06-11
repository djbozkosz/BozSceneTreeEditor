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
			Uint16_3,
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
			StringArray2,
			StringFixed,
			Struct
		};

		sealed struct DialogFile
		{
			QString File;
			QString Filter;
			QString Name;

			inline DialogFile(const QString& file = QString(), const QString& filter = QString(), const QString& name = QString()) :
				File(file), Filter(filter), Name(name)
			{
			}
		};

		typedef QVector<DialogFile> DialogFiles;

		sealed struct NodeFieldType
		{
			ENodeFieldType Type;
			uint           Size;
			QString        Name;

			inline NodeFieldType(const QString& name, ENodeFieldType type, uint size) :
				Type(type), Size(size), Name(name)
			{
			}
		};

		struct StructDefinition;

		sealed struct NodeFieldInfo
		{
			const NodeFieldType* FieldType;
			StructDefinition*    NestedField;
			QString              Name;
			uint                 Number;

			inline NodeFieldInfo() : FieldType(0), NestedField(null), Number(0) {}
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

		typedef QVector<QVector<void*> > StructField;

		sealed struct StructDefinition
		{
			QString                Name;
			QVector<NodeFieldInfo> Fields;
		};

		private: // constants

		static const QVector<NodeFieldType> FIELD_TYPES;

		private: // members

		QRegExp                             m_RDialogFileLine;
		QRegExp                             m_RDialogExportFileLine;
		QRegExp                             m_RNodeLine;
		QRegExp                             m_RStructLine;
		QRegExp                             m_RNodeFieldList;
		QRegExp                             m_RNodeField;
		QRegExp                             m_RNodeFieldLine;
		QRegExp                             m_RNodeNameField;
		QRegExp                             m_RNodeFieldEnumLine;

		DialogFiles                         m_DialogFiles;
		DialogFiles                         m_DialogExportFiles;

		QMap<QString, const NodeFieldType*> m_StringToField;
		QMap<uint, NodeDefinition>          m_Nodes;
		QMap<QString, StructDefinition>     m_Structs;

		QMap<ushort, NodeFieldDefinition>   m_NodeFields;

		typedef QVector<NodeName> NodeNames;
		QMap<ushort, NodeNames>             m_NodeNames;

		typedef QMap<int, QString>                       NodeFieldEnum;
		typedef QMap<ushort, QMap<uint, NodeFieldEnum> > NodeFieldEnums;
		NodeFieldEnums                      m_NodeFieldEnums;

		public: // methods

		explicit Definitions();
		virtual ~Definitions();

		inline const QMap<uint, NodeDefinition>& GetNodes()             const { return m_Nodes;             }
		inline const DialogFiles&                GetDialogFiles()       const { return m_DialogFiles;       }
		inline const DialogFiles&                GetDialogExportFiles() const { return m_DialogExportFiles; }

		const NodeDefinition*      GetNode(ushort parentType, ushort type) const;
		const NodeFieldDefinition* GetNodeField(ushort type) const;
		const NodeDefinition*      GetNodeFieldData(ushort type, uint dataType) const;
		const NodeNames*           GetNodeNames(ushort type) const;
		const NodeFieldEnum*       GetNodeFieldEnum(ushort type, uint fieldIdx) const;

		private: // methods

		void Load(const QString& file);
		void LoadDialogFile(QRegExp& regExp, QVector<DialogFile>& files, const QString& line);
		void LoadNode(const QString& line);
		void LoadStruct(const QString& line);
		void LoadNodeFields(const QString& line);
		void LoadFields(QVector<NodeFieldInfo>& fieldInfos, const QString& fields);
		void LoadField(const QString& field, QString& type, QString& name, uint& size);
		void LoadNodeName(ushort type, const QString& fields);
		void LoadNodeFieldEnum(const QString& line);

		static inline uint GetKey(ushort parentType, ushort type) { return (parentType << 16) | type; }
	};
}}}

#endif // APPLICATION_SCENE_DEFINITIONS_H
