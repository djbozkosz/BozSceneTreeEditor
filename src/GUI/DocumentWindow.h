#ifndef APPLICATION_GUI_DOCUMENTWINDOW_H
#define APPLICATION_GUI_DOCUMENTWINDOW_H

#include <QWidget>
#include <QTreeWidgetItem>

#include "ui_DocumentWindow.h"

#include "Scene/SceneNodeDefinitions.h"


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

			explicit NodeItem(QTreeWidget* parent, Scene::SceneNode* node);
			explicit NodeItem(NodeItem* parent, Scene::SceneNode* node);
			virtual ~NodeItem();

			private:

			static QString GetName(Scene::SceneNode* node);
		};

		private: // members

		Ui::DocumentWindow* m_Ui;

		Document*           m_Document;
		Scene::SceneTree*   m_Tree;


		public: // methods

		explicit DocumentWindow(Document* document, QWidget* parent = null);
		virtual ~DocumentWindow();

		private: // methods

		void SetupTree();
		static void CreateTree(NodeItem *item, Scene::SceneNode* node);

		void SetupTable(Scene::SceneNode* node);
		void SetupTableField(int idx, const void* field, Scene::SceneNodeDefinitions::ENodeFieldType fieldType);

		template <typename T> inline void SetTableFieldInt(int idx, const void* field, int base = 10)
		{
			auto data = reinterpret_cast<const T*>(field);

			auto text = QString::number(*data, base);
			if (base == 16)
			{
				text = QString("0x%1").arg(text);
			}

			m_Ui->Table->setItem(idx, 2, new QTableWidgetItem(text));
		}

		inline void SetTableFieldFloat(int idx, const void* field, int count)
		{
			auto data = reinterpret_cast<const float*>(field);
			for (int i = 0; i < count; i++)
			{
				m_Ui->Table->setItem(idx, i + 2, new QTableWidgetItem(QString::number(data[i])));
			}
		}

		private slots: // handlers

		void UpdateTable(QTreeWidgetItem* current, QTreeWidgetItem* previous);
	};
}}}

#endif // APPLICATION_GUI_DOCUMENTWINDOW_H
