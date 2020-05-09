#ifndef APPLICATION_SCENE_SceneNodeDefinitions_H
#define APPLICATION_SCENE_SceneNodeDefinitions_H

#include <QString>
#include <QRegExp>
#include <QVector>
#include <QMap>

#include "Utility/Base.h"


namespace Djbozkosz {
namespace Application {
namespace Scene
{
	sealed class SceneNodeDefinitions
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
			String
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

			inline NodeFieldInfo() : FieldType(0) {}
		};

		sealed struct NodeDefinition
		{
			ushort                 Type;
			QString                Name;
			bool                   HasChilds;
			QVector<NodeFieldInfo> Fields;

			inline NodeDefinition() : Type(0), HasChilds(false) {}
		};

		private: // constants

		static const QVector<NodeFieldType> FIELD_TYPES;

		private: // members

		QRegExp                             m_RegExtSplitLine;
		QRegExp                             m_RegExtSplitFieldList;
		QRegExp                             m_RegExtSplitField;

		QMap<QString, const NodeFieldType*> m_StringToField;
		QMap<ushort, NodeDefinition>        m_NodeTypeToDefinition;

		public: // methods

		explicit SceneNodeDefinitions();
		virtual ~SceneNodeDefinitions();

		const NodeDefinition* GetDefinition(ushort type) const;

		private: // methods

		void Load(const QString& file);
	};
}}}

#endif // APPLICATION_SCENE_SceneNodeDefinitions_H
