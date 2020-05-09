#include "Utility/PreBase.h"

#include <QStringList>
#include <QTableWidgetItem>

#include "ui_DocumentWindow.h"

#include "GUI/DocumentWindow.h"
#include "Application/Document.h"
#include "Scene/SceneTree.h"
#include "Scene/SceneNode.h"


using namespace Djbozkosz::Application::GUI;
using namespace Djbozkosz::Application::Scene;


DocumentWindow::NodeItem::NodeItem(QTreeWidget* parent, Scene::SceneNode* node) :
	QTreeWidgetItem(parent, QStringList() << GetName(node)),
	Node(node)
{
}

DocumentWindow::NodeItem::NodeItem(NodeItem* parent, Scene::SceneNode* node) :
	QTreeWidgetItem(parent, QStringList() << GetName(node)),
	Node(node)
{
}

DocumentWindow::NodeItem::~NodeItem()
{
}

QString DocumentWindow::NodeItem::GetName(Scene::SceneNode* node)
{
	return (node->Definition != null) ? node->Definition->Name : QString("Unknown node: 0x%1").arg(node->Type, 4, 16);
}


DocumentWindow::DocumentWindow(Document* document, QWidget* parent) :
	QWidget(parent),
	m_Ui(new Ui::DocumentWindow()),
	m_Document(document),
	m_Tree(document->GetTree())
{
	m_Ui->setupUi(this);

	connect(m_Ui->Tree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(UpdateTable(QTreeWidgetItem*,QTreeWidgetItem*)));

	SetupTree();
}

DocumentWindow::~DocumentWindow()
{
	disconnect(m_Ui->Tree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), this, SLOT(UpdateTable(QTreeWidgetItem*,QTreeWidgetItem*)));

	delete m_Ui;
}

void DocumentWindow::SetupTree()
{
	auto tree = m_Ui->Tree;
	tree->clear();

	auto root = m_Tree->Root;
	auto item = new NodeItem(tree, root);
	CreateTree(item, root);
	tree->addTopLevelItem(item);

	tree->expandItem(item);
}

void DocumentWindow::CreateTree(NodeItem* item, Scene::SceneNode* node)
{
	auto childs = node->Childs;
	foreach (child, childs)
	{
		auto childItem = new NodeItem(item, *child);
		CreateTree(childItem, *child);
		item->addChild(childItem);
	}
}

void DocumentWindow::SetupTable(SceneNode* node)
{
	auto table = m_Ui->Table;
	table->clear();

	if (node == null)
		return;

	auto count = node->Fields.size();
	table->setRowCount(count);

	for (int idx = 0; idx < count; idx++)
	{
		if (node->Definition == null)
		{
			table->setItem(idx, 0, new QTableWidgetItem("Unknown data"));
			continue;
		}

		auto const& field     = node->Fields[idx];
		auto const& fieldInfo = node->Definition->Fields[idx];

		table->setItem(idx, 0, new QTableWidgetItem(fieldInfo.Name));
		table->setItem(idx, 1, new QTableWidgetItem(fieldInfo.FieldType->Name));
		table->setItem(idx, 2, new QTableWidgetItem("0"));
	}
}

void DocumentWindow::UpdateTable(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	unused(previous);

	SetupTable(as(current, NodeItem*)->Node);
}
