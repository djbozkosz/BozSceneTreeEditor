#ifndef APPLICATION_SCENE_SCENENODE_H
#define APPLICATION_SCENE_SCENENODE_H

#include <QFile>
#include <QVector>

#include "Utility/Base.h"


class QFile;


namespace Djbozkosz {
namespace Application {
namespace Scene
{
	class SceneNodeDefinitions;


	sealed class SceneNode
	{
		public: // members

		ushort              Type;
		uint                Size;
		QVector<void*>      Fields;
		QVector<SceneNode*> Childs;

		private: // members

		SceneNodeDefinitions* m_NodeDefinitions;

		public: // methods

		SceneNode(SceneNodeDefinitions* nodeDefinitions);
		virtual ~SceneNode();

		bool Load(QFile& reader);
		bool Save(QFile& writer) const;

		private: // methods

		template <typename T> inline bool LoadData(QFile& reader, T& dest)
		{
			auto   sizeToRead = sizeof(T);
			auto   readSize   = reader.read(reinterpret_cast<char*>(&dest), sizeToRead);
			return readSize == sizeToRead;
		}

		inline bool LoadData(QFile& reader, void* dest, uint sizeToRead)
		{
			auto   readSize = reader.read(reinterpret_cast<char*>(dest), sizeToRead);
			return readSize == sizeToRead;
		}
	};
}}}

#endif // APPLICATION_SCENE_SCENENODE_H
