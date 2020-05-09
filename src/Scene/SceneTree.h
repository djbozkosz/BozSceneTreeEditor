#ifndef APPLICATION_SCENE_SCENETREE_H
#define APPLICATION_SCENE_SCENETREE_H

#include <QString>

#include "Utility/Base.h"


namespace Djbozkosz {
namespace Application {
namespace Scene
{
	class Definitions;
	class SceneNode;


	sealed class SceneTree
	{
		public: // members

		SceneNode* Root;

		private: // members

		Definitions* m_Definitions;

		public: // methods

		explicit SceneTree(Definitions* definitions);
		virtual ~SceneTree();

		bool Load(const QString& file);
		bool Save(const QString& file) const;

		private: // methods

		void DeleteTree();
	};
}}}

#endif // APPLICATION_SCENE_SCENETREE_H
