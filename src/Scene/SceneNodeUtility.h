#ifndef APPLICATION_SCENE_SCENENODEUTILITY_H
#define APPLICATION_SCENE_SCENENODEUTILITY_H

#include "Scene/Definitions.h"


namespace Djbozkosz {
namespace Application {
namespace Scene
{
	class SceneNode;


	static_ class SceneNodeUtility
	{
		public: // types

		sealed class FieldContext
		{
			public:

			SceneNode*                        Node;
			uint                              FieldIdx;
			QVector<void*>*                   Fields;
			const Definitions::NodeFieldInfo* FieldInfo;

			explicit inline FieldContext(SceneNode* node, uint fieldIdx, QVector<void*>* fields, const Definitions::NodeFieldInfo* fieldInfo) :
				Node(node), FieldIdx(fieldIdx), Fields(fields), FieldInfo(fieldInfo)
			{
			}

			virtual inline ~FieldContext() {}
		};

		public: // methods

		template <typename T> static inline const T& GetFieldData(const FieldContext& fieldCtx, int idx)
		{
			auto   field = (*fieldCtx.Fields)[fieldCtx.FieldIdx];
			auto   data  = reinterpret_cast<const T*>(field);
			return data[idx];
		}

		template <typename T> static inline void SetFieldData(const FieldContext& fieldCtx, int idx, const T& value)
		{
			auto field = (*fieldCtx.Fields)[fieldCtx.FieldIdx];
			auto data  = reinterpret_cast<T*>(field);
			data[idx]  = value;
		}

		static QString GetEnumValue(const Definitions* definitions, const FieldContext& fieldCtx, int data);

		// operations:
		// get field data at index (float3 ...)
		// set field data at index (fixed and variable node length - realloc)
		// if node length changed:
		// get node path
		// and update node sizes in path
		// add entry into array
		// remove entry from array
		// insert node
		// remove node
		// duplicate node
		// move node
	};
}}}

#endif // APPLICATION_SCENE_SCENENODEUTILITY_H
