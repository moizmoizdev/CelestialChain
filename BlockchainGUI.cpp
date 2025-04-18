//#include "BlockchainGUI.h"
//
//BlockchainGUI::BlockchainGUI(QWidget *parent) : QWidget(parent), difficulty(4) {
//    QVBoxLayout *layout = new QVBoxLayout(this);
//
//    transactionInput = new QLineEdit(this);
//    QPushButton *addTransactionButton = new QPushButton("Add Transaction", this);
//    QPushButton *mineBlockButton = new QPushButton("Mine Block", this);
//    blockchainDisplay = new QLabel(this);
//
//    layout->addWidget(transactionInput);
//    layout->addWidget(addTransactionButton);
//    layout->addWidget(mineBlockButton);
//    layout->addWidget(blockchainDisplay);
//
//    connect(addTransactionButton, &QPushButton::clicked, this, &BlockchainGUI::addTransaction);
//    connect(mineBlockButton, &QPushButton::clicked, this, &BlockchainGUI::mineBlock);
//
//    updateBlockchainDisplay();
//}
//
//void BlockchainGUI::addTransaction() {
//    QString data = transactionInput->text();
//    if (!data.isEmpty()) {
//        mempool.push_back(Transaction(data.toStdString()));
//        transactionInput->clear();
//        updateBlockchainDisplay();
//    }
//}
//
//void BlockchainGUI::mineBlock() {
//    if (!mempool.empty()) {
//        string prevHash = blockchain.empty() ? "0" : blockchain.back().hash;
//        Block newBlock(blockchain.size(), mempool, prevHash, difficulty);
//        blockchain.push_back(newBlock);
//        mempool.clear();
//        updateBlockchainDisplay();
//    }
//}
//
//void BlockchainGUI::updateBlockchainDisplay() {
//    QString displayText;
//    for (const auto& block : blockchain) {
//        displayText += QString("Block %1:\nHash: %2\nPrevious Hash: %3\n\n")
//            .arg(block.blockNumber)
//            .arg(QString::fromStdString(block.hash))
//            .arg(QString::fromStdString(block.previousHash));
//    }
//    blockchainDisplay->setText(displayText);
//} 