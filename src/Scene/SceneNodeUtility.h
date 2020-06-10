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

		typedef QVector<SceneNode*> NodePath;

		sealed class FieldContext
		{
			public:

			SceneNode*                                 Node;
			uint                                       FieldIdx;
			QVector<void*>*                            Fields;
			const QVector<Definitions::NodeFieldInfo>* FieldInfos;

			explicit inline FieldContext(SceneNode* node, uint fieldIdx, QVector<void*>* fields, const QVector<Definitions::NodeFieldInfo>* fieldInfos) :
				Node(node), FieldIdx(fieldIdx), Fields(fields), FieldInfos(fieldInfos)
			{
			}

			virtual inline ~FieldContext() {}

			inline void*& GetField()        const { return (*Fields)[FieldIdx]; }
			inline void*& GetField(int idx) const { return (*Fields)[idx];      }

			inline const Definitions::NodeFieldInfo* GetFieldInfo()        const { return &(*FieldInfos)[FieldIdx]; }
			inline const Definitions::NodeFieldInfo* GetFieldInfo(int idx) const { return &(*FieldInfos)[idx];      }
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
				data << QString::number(field);
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

		public: // node data methods

		static uint CreateFieldsData(QVector<void*>& fields, const QVector<Definitions::NodeFieldInfo>& fieldInfos);
		static void DestroyFieldsData(QVector<void*>& fields, const QVector<Definitions::NodeFieldInfo>* fieldInfos);
		static uint GetFieldSize(const void* field, const Definitions::NodeFieldInfo* fieldInfo, bool withHeader);
		static uint GetFieldsSize(const QVector<void*>& fields, const QVector<Definitions::NodeFieldInfo>& fieldInfos);

		public: // node methods

		static QString GetNodeName(SceneNode* node, const Definitions* definitions);
		static bool MoveNode(SceneNode* node, SceneNode* root, SceneNode* parent, SceneNode* newParent, uint oldIdx, uint newIdx);

		public: // tree methods

		static bool GetNodePath(NodePath& path, SceneNode* parent, const SceneNode* node);
		static void ApplyNodeSizeOffset(NodePath& path, int offset);
	};
}}}

#endif // APPLICATION_SCENE_SCENENODEUTILITY_H
