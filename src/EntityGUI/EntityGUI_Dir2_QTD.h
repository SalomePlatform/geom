/********************************************************************************
** Form generated from reading ui file 'EntityGUI_Dir2_QTD.ui'
**
** Created: Tue Jul 10 10:24:41 2007
**      by: Qt User Interface Compiler version 4.2.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef ENTITYGUI_DIR2_QTD_H
#define ENTITYGUI_DIR2_QTD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QWidget>
#include <Qt3Support/Q3MimeSourceFactory>

class Ui_EntityGUI_Dir2_QTD
{
public:
    QGridLayout *gridLayout;
    QGroupBox *GroupDir2;
    QGridLayout *gridLayout1;
    QGridLayout *gridLayout2;
    QRadioButton *RB_Dir22;
    QRadioButton *RB_Dir23;
    QSpacerItem *spacerItem;
    QRadioButton *RB_Dir21;

    void setupUi(QWidget *EntityGUI_Dir2_QTD)
    {
    EntityGUI_Dir2_QTD->setObjectName(QString::fromUtf8("EntityGUI_Dir2_QTD"));
    gridLayout = new QGridLayout(EntityGUI_Dir2_QTD);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    GroupDir2 = new QGroupBox(EntityGUI_Dir2_QTD);
    GroupDir2->setObjectName(QString::fromUtf8("GroupDir2"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(7));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(GroupDir2->sizePolicy().hasHeightForWidth());
    GroupDir2->setSizePolicy(sizePolicy);
    gridLayout1 = new QGridLayout(GroupDir2);
    gridLayout1->setSpacing(6);
    gridLayout1->setMargin(11);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    gridLayout2 = new QGridLayout();
    gridLayout2->setSpacing(6);
    gridLayout2->setMargin(0);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    RB_Dir22 = new QRadioButton(GroupDir2);
    RB_Dir22->setObjectName(QString::fromUtf8("RB_Dir22"));

    gridLayout2->addWidget(RB_Dir22, 1, 0, 1, 1);

    RB_Dir23 = new QRadioButton(GroupDir2);
    RB_Dir23->setObjectName(QString::fromUtf8("RB_Dir23"));

    gridLayout2->addWidget(RB_Dir23, 2, 0, 1, 1);

    spacerItem = new QSpacerItem(0, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

    gridLayout2->addItem(spacerItem, 3, 0, 1, 1);

    RB_Dir21 = new QRadioButton(GroupDir2);
    RB_Dir21->setObjectName(QString::fromUtf8("RB_Dir21"));

    gridLayout2->addWidget(RB_Dir21, 0, 0, 1, 1);


    gridLayout1->addLayout(gridLayout2, 0, 0, 1, 1);


    gridLayout->addWidget(GroupDir2, 0, 0, 1, 1);

    QWidget::setTabOrder(RB_Dir21, RB_Dir22);
    QWidget::setTabOrder(RB_Dir22, RB_Dir23);

    retranslateUi(EntityGUI_Dir2_QTD);

    QSize size(124, 106);
    size = size.expandedTo(EntityGUI_Dir2_QTD->minimumSizeHint());
    EntityGUI_Dir2_QTD->resize(size);


    QMetaObject::connectSlotsByName(EntityGUI_Dir2_QTD);
    } // setupUi

    void retranslateUi(QWidget *EntityGUI_Dir2_QTD)
    {
    EntityGUI_Dir2_QTD->setWindowTitle(QApplication::translate("EntityGUI_Dir2_QTD", "EntityGUI_Dir2_QTD", 0, QApplication::UnicodeUTF8));
    GroupDir2->setTitle(QApplication::translate("EntityGUI_Dir2_QTD", "Direction", 0, QApplication::UnicodeUTF8));
    RB_Dir22->setText(QApplication::translate("EntityGUI_Dir2_QTD", "X", 0, QApplication::UnicodeUTF8));
    RB_Dir23->setText(QApplication::translate("EntityGUI_Dir2_QTD", "Y", 0, QApplication::UnicodeUTF8));
    RB_Dir21->setText(QApplication::translate("EntityGUI_Dir2_QTD", "Length", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(EntityGUI_Dir2_QTD);
    } // retranslateUi

};

namespace Ui {
    class EntityGUI_Dir2_QTD: public Ui_EntityGUI_Dir2_QTD {};
} // namespace Ui

#endif // ENTITYGUI_DIR2_QTD_H
