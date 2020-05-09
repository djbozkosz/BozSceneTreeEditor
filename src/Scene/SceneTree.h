#ifndef APPLICATION_SCENE_SCENETREE_H
#define APPLICATION_SCENE_SCENETREE_H

#include <QString>

#include "Utility/Base.h"


namespace Djbozkosz {
namespace Application {
namespace Scene
{
	class SceneNodeDefinitions;
	class SceneNode;


	sealed class SceneTree
	{
		public: // members

		SceneNode* Root;

		private: // members

		SceneNodeDefinitions* m_NodeDefinitions;

		public: // methods

		explicit SceneTree(SceneNodeDefinitions* nodeDefinitions);
		virtual ~SceneTree();

		bool Load(const QString& file);
		bool Save(const QString& file) const;

		private: // methods

		void DeleteTree();
	};
}}}

#endif // APPLICATION_SCENE_SCENETREE_H
