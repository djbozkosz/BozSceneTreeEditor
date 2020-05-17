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

		private: // members

		Ui::DocumentWindow* m_Ui;

		Document*           m_Document;
		Scene::SceneTree*   m_Tree;
		Scene::Definitions* m_Definitions;


		public: // methods

		explicit DocumentWindow(Document* document, Scene::Definitions* definitions, QWidget* parent = null);
		virtual ~DocumentWindow();

		private: // methods

		void SetupTree();
		void CreateTree(NodeItem *item, Scene::SceneNode* node);

		void SetupTable(Scene::SceneNode* node);
		void SetupTableField(ushort type, int& idx, const void* field, const Scene::Definitions::NodeFieldInfo& fieldInfo);

		template <typename T> inline int SetTableFieldInt(int idx, const void* field, int base = 10)
		{
			auto data = reinterpret_cast<const T*>(field);

			auto text = QString::number(*data, base);
			if (base == 16)
			{
				text = QString("0x%1").arg(text);
			}

			m_Ui->Table->setItem(idx, 2, new QTableWidgetItem(text));
			return *data;
		}

		inline void SetTableFieldFloat(int idx, const void* field, int count)
		{
			auto data = reinterpret_cast<const float*>(field);
			for (int i = 0; i < count; i++)
			{
				m_Ui->Table->setItem(idx, i + 2, new QTableWidgetItem(QString::number(data[i])));
			}
		}

		QString GetNodeName(Scene::SceneNode* node) const;

		private slots: // handlers

		void UpdateTable(QTreeWidgetItem* current, QTreeWidgetItem* previous);
	};
}}}

#endif // APPLICATION_GUI_DOCUMENTWINDOW_H
