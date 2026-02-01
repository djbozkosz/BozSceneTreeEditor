#ifndef APPLICATION_SCENE_SCENENODE_H
#define APPLICATION_SCENE_SCENENODE_H

#include <QFile>
#include <QVariant>
#include <QVariantList>
#include <QVector>

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

		public: // methods

		explicit SceneNode();
		virtual ~SceneNode();

		SceneNode* GetChild(ushort type) const;
	};
}}}

#endif // APPLICATION_SCENE_SCENENODE_H
