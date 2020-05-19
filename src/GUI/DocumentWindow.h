#ifndef APPLICATION_GUI_DOCUMENTWINDOW_H
#define APPLICATION_GUI_DOCUMENTWINDOW_H

#include <QWidget>
#include <QTreeWidgetItem>

#include "ui_DocumentWindow.h"

#include "Scene/Definitions.h"


class QTreeWidget;
class QTreeWidgetItem;


namespace Ui
{
	class DocumentWindow;
}


namespace Djbozkosz {
namespace Application
{
class Document;


namespace Scene
{
	class SceneNode;
	class SceneTree;
}


namespace GUI
{
	sealed class DocumentWindow : public QWidget
	{											 
		private:

		Q_OBJECT

		private: // types

		sealed class NodeItem : public QTreeWidgetItem
		{
			public:

			Scene::SceneNode* Node;

			explicit NodeItem(QTreeWidget* parent, Scene::SceneNode* node, QString& name);
			explicit NodeItem(NodeItem* parent, Scene::SceneNode* node, QString& name);
			virtual ~NodeItem();
		};

		sealed class FieldItem : public QTableWidgetItem
		{
			public:

			uint                                     FieldIdx;
			QVector<void*>*                          Fields;
			const Scene::Definitions::NodeFieldInfo* FieldInfo;

			explicit inline FieldItem(const QString& text, uint fieldIdx, QVector<void*>* fields, const Scene::Definitions::NodeFieldInfo* fieldInfo) :
				QTableWidgetItem(text), FieldIdx(fieldIdx), Fields(fields), FieldInfo(fieldInfo)
			{
			}

			virtual inline ~FieldItem() {}
		};

		sealed class ReadOnlyItem : public QTableWidgetItem
		{
			public:

			explicit inline ReadOnlyItem(const QString& text) : QTableWidgetItem(text)
			{
				setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			}

			virtual inline ~ReadOnlyItem() {}
		};

		private: // members

		Ui::DocumentWindow* m_Ui;

		Document*           m_Document;
		Scene::SceneTree*   m_Tree;
		Scene::Definitions* m_Definitions;


		public: // methods

		explicit DocumentWindow(Document* document, Scene::Definitions* definitions, QWidget* parent = null);
		virtual ~DocumentWindow();

		inline Document* GetDocument() const { return m_Document; }

		private: // methods

		void SetupTree();
		void CreateTree(NodeItem *item, Scene::SceneNode* node);

		void SetupTable(Scene::SceneNode* node);
		void SetupTableField(ushort type, int& row, uint fieldIdx, QVector<void*>* fields, const Scene::Definitions::NodeFieldInfo* fieldInfo);

		template <typename T> inline int SetTableFieldInt(int row, uint fieldIdx, QVector<void*>* fields, const Scene::Definitions::NodeFieldInfo* fieldInfo, int count, int base = 10)
		{
			auto field = (*fields)[fieldIdx];
			auto data  = reinterpret_cast<const T*>(field);
			for (int i = 0; i < count; i++)
			{
				auto text = QString::number(data[i], base);
				if (base == 16)
				{
					text = QString("0x%1").arg(text);
				}

				m_Ui->Table->setItem(row, i + 2, new FieldItem(text, fieldIdx, fields, fieldInfo));
			}

			return data[0];
		}

		inline void SetTableFieldFloat(int idx, uint fieldIdx, QVector<void*>* fields, const Scene::Definitions::NodeFieldInfo* fieldInfo, int count)
		{
			auto field = (*fields)[fieldIdx];
			auto data  = reinterpret_cast<const float*>(field);
			for (int i = 0; i < count; i++)
			{
				auto text = QString::number(data[i]);
				m_Ui->Table->setItem(idx, i + 2, new FieldItem(text, fieldIdx, fields, fieldInfo));
			}
		}

		template <typename T> inline void SetField(const T& value, void* field, uint offset = 0)
		{
			auto data = reinterpret_cast<T*>(field);
			data[offset] = value;
		}

		QString GetNodeName(Scene::SceneNode* node) const;

		private slots: // handlers

		void UpdateTable(QTreeWidgetItem* current, QTreeWidgetItem* previous);
		void UpdateField(QTableWidgetItem* item);
	};
}}}

#endif // APPLICATION_GUI_DOCUMENTWINDOW_H
