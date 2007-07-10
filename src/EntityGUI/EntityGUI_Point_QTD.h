/********************************************************************************
** Form generated from reading ui file 'EntityGUI_Point_QTD.ui'
**
** Created: Mon Jul 9 17:24:53 2007
**      by: Qt User Interface Compiler version 4.2.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef ENTITYGUI_POINT_QTD_H
#define ENTITYGUI_POINT_QTD_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QRadioButton>
#include <QtGui/QWidget>
#include <Qt3Support/Q3MimeSourceFactory>

class Ui_EntityGUI_Point_QTD
{
public:
    QGridLayout *gridLayout;
    QGroupBox *GroupPoint;
    QGridLayout *gridLayout1;
    QGridLayout *gridLayout2;
    QRadioButton *RB_Point2;
    QRadioButton *RB_Point1;
    QRadioButton *RB_Point3;

    void setupUi(QWidget *EntityGUI_Point_QTD)
    {
    EntityGUI_Point_QTD->setObjectName(QString::fromUtf8("EntityGUI_Point_QTD"));
    gridLayout = new QGridLayout(EntityGUI_Point_QTD);
    gridLayout->setSpacing(6);
    gridLayout->setMargin(0);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    GroupPoint = new QGroupBox(EntityGUI_Point_QTD);
    GroupPoint->setObjectName(QString::fromUtf8("GroupPoint"));
    QSizePolicy sizePolicy(static_cast<QSizePolicy::Policy>(7), static_cast<QSizePolicy::Policy>(0));
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(GroupPoint->sizePolicy().hasHeightForWidth());
    GroupPoint->setSizePolicy(sizePolicy);
    gridLayout1 = new QGridLayout(GroupPoint);
    gridLayout1->setSpacing(6);
    gridLayout1->setMargin(11);
    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
    gridLayout2 = new QGridLayout();
    gridLayout2->setSpacing(6);
    gridLayout2->setMargin(0);
    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
    RB_Point2 = new QRadioButton(GroupPoint);
    RB_Point2->setObjectName(QString::fromUtf8("RB_Point2"));

    gridLayout2->addWidget(RB_Point2, 1, 0, 1, 1);

    RB_Point1 = new QRadioButton(GroupPoint);
    RB_Point1->setObjectName(QString::fromUtf8("RB_Point1"));

    gridLayout2->addWidget(RB_Point1, 0, 0, 1, 1);

    RB_Point3 = new QRadioButton(GroupPoint);
    RB_Point3->setObjectName(QString::fromUtf8("RB_Point3"));

    gridLayout2->addWidget(RB_Point3, 2, 0, 1, 1);


    gridLayout1->addLayout(gridLayout2, 0, 0, 1, 1);


    gridLayout->addWidget(GroupPoint, 0, 0, 1, 1);

    QWidget::setTabOrder(RB_Point1, RB_Point2);
    QWidget::setTabOrder(RB_Point2, RB_Point3);

    retranslateUi(EntityGUI_Point_QTD);

    QSize size(124, 106);
    size = size.expandedTo(EntityGUI_Point_QTD->minimumSizeHint());
    EntityGUI_Point_QTD->resize(size);


    QMetaObject::connectSlotsByName(EntityGUI_Point_QTD);
    } // setupUi

    void retranslateUi(QWidget *EntityGUI_Point_QTD)
    {
    EntityGUI_Point_QTD->setWindowTitle(QApplication::translate("EntityGUI_Point_QTD", "EntityGUI_Point_QTD", 0, QApplication::UnicodeUTF8));
    GroupPoint->setTitle(QApplication::translate("EntityGUI_Point_QTD", "Point", 0, QApplication::UnicodeUTF8));
    RB_Point2->setText(QApplication::translate("EntityGUI_Point_QTD", "Relative", 0, QApplication::UnicodeUTF8));
    RB_Point1->setText(QApplication::translate("EntityGUI_Point_QTD", "Absolute", 0, QApplication::UnicodeUTF8));
    RB_Point3->setText(QApplication::translate("EntityGUI_Point_QTD", "Selection", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(EntityGUI_Point_QTD);
    } // retranslateUi

};

namespace Ui {
    class EntityGUI_Point_QTD: public Ui_EntityGUI_Point_QTD {};
} // namespace Ui

#endif // ENTITYGUI_POINT_QTD_H
