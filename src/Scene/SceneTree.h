#ifndef APPLICATION_SCENE_SCENETREE_H
#define APPLICATION_SCENE_SCENETREE_H

#include <QString>

#include "Utility/Base.h"


class QFile;


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

		bool Load(QFile& reader);
		bool Save(QFile& writer) const;

		private: // methods

		void DeleteTree();
	};
}}}

#endif // APPLICATION_SCENE_SCENETREE_H
