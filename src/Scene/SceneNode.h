#ifndef APPLICATION_SCENE_SCENENODE_H
#define APPLICATION_SCENE_SCENENODE_H

#include <QFile>
#include <QVector>
#include <QVariant>
#include <QVariantList>

#include "Scene/Definitions.h"


class QFile;


namespace Djbozkosz {
namespace Application {
namespace Scene
{
	sealed class SceneNode
	{
		public: // members

		ushort              Type;
		uint                Size;
		QVector<void*>      Fields;
		QVector<SceneNode*> Childs;

		const Definitions::NodeDefinition* Definition;

		private: // members

		Definitions* m_Definitions;

		public: // methods

		explicit SceneNode(Definitions* definitions);
		virtual ~SceneNode();

		SceneNode* GetChild(ushort type) const;

		bool Load(QFile& reader, SceneNode* parent);
		bool Save(QFile& writer) const;

		private: // methods

		static void LoadFields(QFile& reader,       QVector<void*>& fields, const QVector<Definitions::NodeFieldInfo>& fieldInfos);
		static void SaveFields(QFile& writer, const QVector<void*>& fields, const QVector<Definitions::NodeFieldInfo>& fieldInfos);

		template <typename T> static inline bool LoadData(QFile& reader, T& dest)
		{
			return LoadData(reader, &dest, sizeof(T));
		}

		static inline bool LoadData(QFile& reader, void* dest, uint sizeToRead)
		{
			auto   readSize = reader.read(reinterpret_cast<char*>(dest), sizeToRead);
			return readSize == sizeToRead;
		}

		template <typename T> static inline bool SaveData(QFile& writer, const T& src)
		{
			return SaveData(writer, &src, sizeof(T));
		}

		static inline bool SaveData(QFile& writer, const void* src, uint sizeToWrite)
		{
			auto   writtenSize = writer.write(reinterpret_cast<const char*>(src), sizeToWrite);
			return writtenSize == sizeToWrite;
		}
	};
}}}

#endif // APPLICATION_SCENE_SCENENODE_H
