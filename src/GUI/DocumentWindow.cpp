#include "Utility/PreBase.h"

#include <QStringList>
#include <QTableWidgetItem>

#include "GUI/DocumentWindow.h"
#include "Application/Document.h"
#include "Scene/SceneTree.h"
#include "Scene/SceneNode.h"


using namespace Djbozkosz::Application::GUI;
using namespace Djbozkosz::Application::Scene;


DocumentWindow::NodeItem::NodeItem(QTreeWidget* parent, Scene::SceneNode* node, QString& name) :
	QTreeWidgetItem(parent, QStringList() << name),
	Node(node)
{
}

DocumentWindow::NodeItem::NodeItem(NodeItem* parent, Scene::SceneNode* node, QString& name) :
	QTreeWidgetItem(parent, QStringList() << name),
	Node(node)
{
}

DocumentWindow::NodeItem::~NodeItem()
{
}


DocumentWindow::DocumentWindow(Document* document, Definitions* definitions, QWidget* parent) :
	QWidget(parent),
	m_Ui(new Ui::DocumentWindow()),
	m_Document(document),
	m_Tree(document->GetTree()),
	m_Definitions(definitions)
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
	auto name = GetNodeName(root);
	auto item = new NodeItem(tree, root, name);
	CreateTree(item, root);
	tree->addTopLevelItem(item);

	tree->expandItem(item);
}

void DocumentWindow::CreateTree(NodeItem* item, Scene::SceneNode* node)
{
	auto childs = node->Childs;
	foreach (child, childs)
	{
		auto childName = GetNodeName(*child);
		auto childItem = new NodeItem(item, *child, childName);
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

	if (node->Definition == null || node->Definition->Fields[0].FieldType->Type == Definitions::ENodeFieldType::Unknown)
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
		SetupTableField(node->Type, idx, field, fieldInfo.FieldType->Type);
	}
}

void DocumentWindow::SetupTableField(ushort type, int idx, const void* field, Definitions::ENodeFieldType fieldType)
{
	int intValue = -1;

	switch (fieldType)
	{
		case Definitions::ENodeFieldType::Uint8:  intValue = SetTableFieldInt<uchar >(idx, field    ); break;
		case Definitions::ENodeFieldType::Uint16: intValue = SetTableFieldInt<ushort>(idx, field    ); break;
		case Definitions::ENodeFieldType::Uint32: intValue = SetTableFieldInt<uint  >(idx, field    ); break;
		case Definitions::ENodeFieldType::Int8:   intValue = SetTableFieldInt<char  >(idx, field    ); break;
		case Definitions::ENodeFieldType::Int16:  intValue = SetTableFieldInt<short >(idx, field    ); break;
		case Definitions::ENodeFieldType::Int32:  intValue = SetTableFieldInt<int   >(idx, field    ); break;
		case Definitions::ENodeFieldType::Hex8:   intValue = SetTableFieldInt<uchar >(idx, field, 16); break;
		case Definitions::ENodeFieldType::Hex16:  intValue = SetTableFieldInt<ushort>(idx, field, 16); break;
		case Definitions::ENodeFieldType::Hex32:  intValue = SetTableFieldInt<uint  >(idx, field, 16); break;
		case Definitions::ENodeFieldType::Float:             SetTableFieldFloat      (idx, field, 1 ); break;
		case Definitions::ENodeFieldType::Float2:            SetTableFieldFloat      (idx, field, 2 ); break;
		case Definitions::ENodeFieldType::Float3:            SetTableFieldFloat      (idx, field, 3 ); break;
		case Definitions::ENodeFieldType::Float4:            SetTableFieldFloat      (idx, field, 4 ); break;
		case Definitions::ENodeFieldType::Color:             SetTableFieldFloat      (idx, field, 3 ); break;

		case Definitions::ENodeFieldType::String:
			m_Ui->Table->setItem(idx, 2, new QTableWidgetItem(QString::fromLatin1(reinterpret_cast<const char*>(field))));
			break;

		default:
			break;
	}

	// hack begin
	if (fieldType >= Definitions::ENodeFieldType::Uint8 && fieldType <= Definitions::ENodeFieldType::Hex32)
	{
		auto enumMap = m_Definitions->GetNodeFieldEnum(type, idx);
		if (enumMap != null)
		{
			auto const& enumValue = enumMap->find(intValue);
			if (enumValue != enumMap->end())
			{
				auto item = m_Ui->Table->item(idx, 2);
				item->setText(QString("%1 [%2]").arg(intValue).arg(enumValue.value()));
			}
		}
	}
	// hack end
}

QString DocumentWindow::GetNodeName(SceneNode* node) const
{
	if (node->Definition == null)
		return QString("Unknown node: 0x%1").arg(node->Type, 4, 16);

	auto nameInfo = m_Definitions->GetNodeName(node->Type);
	if (nameInfo != null)
	{
		auto childType = nameInfo->ChildType;
		auto nodeName  = (childType > 0) ? node->GetChild(childType) : node;

		QString name;

		if (nodeName != null)
		{
			name = QString::fromLatin1(reinterpret_cast<const char*>(nodeName->Fields[nameInfo->FieldIdx]));
		}

		return QString("%1 [%2]").arg(node->Definition->Name, name);
	}

	return node->Definition->Name;
}

void DocumentWindow::UpdateTable(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	unused(previous);

	SetupTable(as(current, NodeItem*)->Node);
}
