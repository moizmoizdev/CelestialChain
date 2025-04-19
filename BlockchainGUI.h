//#ifndef BLOCKCHAIN_GUI_H
//#define BLOCKCHAIN_GUI_H
//
//#include <QWidget>
//#include <QLineEdit>
//#include <QPushButton>
//#include <QLabel>
//#include <QVBoxLayout>
//#include "Block.h"
//
//class BlockchainGUI : public QWidget {
//    Q_OBJECT
//
//public:
//    BlockchainGUI(QWidget *parent = nullptr);
//
//private slots:
//    void addTransaction();
//    void mineBlock();
//
//private:
//    QLineEdit *transactionInput;
//    QLabel *blockchainDisplay;
//    vector<Block> blockchain;
//    vector<Transaction> mempool;
//    int difficulty;
//
//    void updateBlockchainDisplay();
//};
//
//#endif // BLOCKCHAIN_GUI_H 