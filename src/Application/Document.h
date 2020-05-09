#ifndef APPLICATION_SCENE_DOCUMENT_H
#define APPLICATION_SCENE_DOCUMENT_H

#include <QString>

#include "Utility/Base.h"


namespace Djbozkosz {
namespace Application {
namespace Scene
{
	class SceneTree;
	class SceneNodeDefinitions;
}


	sealed class Document
	{
		private: // members

		QString           m_File;

		Scene::SceneTree* m_Tree;

		public: // methods

		explicit Document(Scene::SceneNodeDefinitions* nodeDefinitions);
		virtual ~Document();

		void Load(const QString& file);

		inline const QString&          GetFile() const { return m_File; }
		inline       Scene::SceneTree* GetTree() const { return m_Tree; }
	};
}}

#endif // APPLICATION_SCENE_DOCUMENT_H
