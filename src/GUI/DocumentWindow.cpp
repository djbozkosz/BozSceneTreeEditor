#include "Utility/PreBase.h"

#include <QStringList>
#include <QTableWidgetItem>

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
	table->setRowCount(0);

	if (node == null)
		return;

	auto count = node->Fields.size();
	if (count == 0)
	{
		table->setRowCount(1);
		table->setItem(0, 0, new QTableWidgetItem("No data"));
		return;
	}

	if (node->Definition == null || node->Definition->Fields[0].FieldType->Type == SceneNodeDefinitions::ENodeFieldType::Unknown)
	{
		table->setRowCount(1);
		table->setItem(0, 0, new QTableWidgetItem("Unknown data"));
		return;
	}

	for (int idx = 0; idx < count; idx++)
	{
		auto const& field     = node->Fields[idx];
		auto const& fieldInfo = node->Definition->Fields[idx];

		table->setRowCount(table->rowCount() + 1);
		table->setItem(idx, 0, new QTableWidgetItem(fieldInfo.Name));
		table->setItem(idx, 1, new QTableWidgetItem(fieldInfo.FieldType->Name));
		SetupTableField(idx, field, fieldInfo.FieldType->Type);
	}
}

void DocumentWindow::SetupTableField(int idx, const void* field, SceneNodeDefinitions::ENodeFieldType fieldType)
{
	switch (fieldType)
	{
		case SceneNodeDefinitions::ENodeFieldType::Uint8:  SetTableFieldInt<uchar >(idx, field    ); return;
		case SceneNodeDefinitions::ENodeFieldType::Uint16: SetTableFieldInt<ushort>(idx, field    ); return;
		case SceneNodeDefinitions::ENodeFieldType::Uint32: SetTableFieldInt<uint  >(idx, field    ); return;
		case SceneNodeDefinitions::ENodeFieldType::Int8:   SetTableFieldInt<char  >(idx, field    ); return;
		case SceneNodeDefinitions::ENodeFieldType::Int16:  SetTableFieldInt<short >(idx, field    ); return;
		case SceneNodeDefinitions::ENodeFieldType::Int32:  SetTableFieldInt<int   >(idx, field    ); return;
		case SceneNodeDefinitions::ENodeFieldType::Hex8:   SetTableFieldInt<uchar >(idx, field, 16); return;
		case SceneNodeDefinitions::ENodeFieldType::Hex16:  SetTableFieldInt<ushort>(idx, field, 16); return;
		case SceneNodeDefinitions::ENodeFieldType::Hex32:  SetTableFieldInt<uint  >(idx, field, 16); return;
		case SceneNodeDefinitions::ENodeFieldType::Float:  SetTableFieldFloat      (idx, field, 1 ); return;
		case SceneNodeDefinitions::ENodeFieldType::Float2: SetTableFieldFloat      (idx, field, 2 ); return;
		case SceneNodeDefinitions::ENodeFieldType::Float3: SetTableFieldFloat      (idx, field, 3 ); return;
		case SceneNodeDefinitions::ENodeFieldType::Float4: SetTableFieldFloat      (idx, field, 4 ); return;
		case SceneNodeDefinitions::ENodeFieldType::Color:  SetTableFieldFloat      (idx, field, 3 ); return;

		case SceneNodeDefinitions::ENodeFieldType::String:
			m_Ui->Table->setItem(idx, 2, new QTableWidgetItem(QString::fromLatin1(reinterpret_cast<const char*>(field))));
			return;

		default:
			return;
	}
}

void DocumentWindow::UpdateTable(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	unused(previous);

	SetupTable(as(current, NodeItem*)->Node);
}
