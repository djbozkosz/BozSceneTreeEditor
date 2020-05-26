#ifndef APPLICATION_SCENE_SCENENODESERIALIZER_H
#define APPLICATION_SCENE_SCENENODESERIALIZER_H

#include <QVector>
#include <QIODevice>

#include "Scene/Definitions.h"


namespace Djbozkosz {
namespace Application {
namespace Scene
{
	class SceneNode;


	static_ class SceneNodeSerializer
	{
		public: // methods

		static SceneNode* Deserialize(QIODevice& reader, SceneNode* parent, const Definitions& definitions);
		static void       Serialize  (QIODevice& writer, const SceneNode& sceneNode);

		private: // methods

		static void DeserializeFields(QIODevice& reader,       QVector<void*>& fields, const QVector<Definitions::NodeFieldInfo>& fieldInfos);
		static void SerializeFields  (QIODevice& writer, const QVector<void*>& fields, const QVector<Definitions::NodeFieldInfo>& fieldInfos);

		template <typename T> static inline bool DeserializeData(QIODevice& reader, T& dest)
		{
			return DeserializeData(reader, &dest, sizeof(T));
		}

		static inline bool DeserializeData(QIODevice& reader, void* dest, uint sizeToRead)
		{
			auto   readSize = reader.read(reinterpret_cast<char*>(dest), sizeToRead);
			return readSize == sizeToRead;
		}

		template <typename T> static inline bool SerializeData(QIODevice& writer, const T& src)
		{
			return SerializeData(writer, &src, sizeof(T));
		}

		static inline bool SerializeData(QIODevice& writer, const void* src, uint sizeToWrite)
		{
			auto   writtenSize = writer.write(reinterpret_cast<const char*>(src), sizeToWrite);
			return writtenSize == sizeToWrite;
		}
	};
}}}

#endif // APPLICATION_SCENE_SCENENODESERIALIZER_H
