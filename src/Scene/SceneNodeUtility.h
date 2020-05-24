#ifndef APPLICATION_SCENE_SCENENODEUTILITY_H
#define APPLICATION_SCENE_SCENENODEUTILITY_H

#include <QStringList>
#include <QVector>

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

		public: // get field methods

		template <typename T> static inline const T& GetFieldData(const FieldContext& fieldCtx, int idx)
		{
			auto   field = (*fieldCtx.Fields)[fieldCtx.FieldIdx];
			auto   data  = reinterpret_cast<const T*>(field);
			return data[idx];
		}

		template <typename T> static inline void GetFieldDataInt(QStringList& data, const SceneNodeUtility::FieldContext& fieldCtx, int count)
		{
			for (int idx = 0; idx < count; idx++)
			{
				auto field = GetFieldData<T>(fieldCtx, idx);
				data << QString::number(field);
			}
		}

		template <typename T> static inline void GetFieldDataHex(QStringList& data, const SceneNodeUtility::FieldContext& fieldCtx, int count)
		{
			for (int idx = 0; idx < count; idx++)
			{
				auto field = GetFieldData<T>(fieldCtx, idx);
				data << QString("0x%2").arg(field, 0, 16);
			}
		}

		static inline void GetFieldDataFloat(QStringList& data, const SceneNodeUtility::FieldContext& fieldCtx, int count)
		{
			for (int idx = 0; idx < count; idx++)
			{
				auto field = SceneNodeUtility::GetFieldData<float>(fieldCtx, idx);
				data << QString::number(field, 'g', 3);
			}
		}

		static QStringList GetFieldDataAsString(const FieldContext& fieldCtx);
		static QString     GetFieldDataEnum(const Definitions* definitions, const FieldContext& fieldCtx, int idx);

		public: // set field methods

		template <typename T> static inline void SetFieldData(const FieldContext& fieldCtx, int idx, const T& value)
		{
			auto field = (*fieldCtx.Fields)[fieldCtx.FieldIdx];
			auto data  = reinterpret_cast<T*>(field);
			data[idx]  = value;
		}

		static void* ResizeFieldData(const FieldContext& fieldCtx, int dataSize);

		static void SetFieldDataFromString(SceneNode* root, const FieldContext& fieldCtx, const QString& data, int idx);

		public: // tree methods

		static bool GetNodePath(QVector<SceneNode*>& path, SceneNode* parent, const SceneNode* node);
		static void ApplyNodeSizeOffset(QVector<SceneNode*>& path, int offset);

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
