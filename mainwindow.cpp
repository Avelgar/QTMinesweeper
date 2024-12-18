#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QDebug>
#include <cstdlib>
#include <QMessageBox>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , elapsedTime(0)
    , dynamicArray(nullptr)
    , ROW(0)
    , COL(0)
    , mines(0)
    , start(true)
    , win(false)
{
    ui->setupUi(this);
    ui->DifficultyComboBox->addItems({"Лёгкая", "Средняя", "Сложная"});

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTimer);
}

MainWindow::~MainWindow()
{

    if (dynamicArray != nullptr) {
        for (int i = 0; i < ROW; ++i) {
            delete[] dynamicArray[i];
        }
        delete[] dynamicArray;
    }

    delete ui;
}

void MainWindow::on_StartButton_clicked()
{
    ui->label_2->setText("");
    ui->label_time->setText("0");
    elapsedTime = 0;
    timer->start(1000);
    start = true;
    int DifficultyIndex = ui->DifficultyComboBox->currentIndex();

    if (dynamicArray != nullptr) {
        for (int i = 0; i < ROW; ++i) {
            delete[] dynamicArray[i];
        }
        delete[] dynamicArray;
    }

    if (DifficultyIndex == 0) {
        ROW = 8;
        COL = ROW;
        mines = 10;
    } else if (DifficultyIndex == 1) {
        ROW = 16;
        COL = ROW;
        mines = 40;
    } else {
        ROW = 30;
        COL = 16;
        mines = 60;
    }

    dynamicArray = new int*[ROW];
    for (int i = 0; i < ROW; ++i) {
        dynamicArray[i] = new int[COL]();
    }

    ui->tableWidget->horizontalHeader()->setVisible(false);
    ui->tableWidget->verticalHeader()->setVisible(false);

    ui->tableWidget->setRowCount(ROW);
    ui->tableWidget->setColumnCount(COL);

    ui->tableWidget->clear();

    int buttonSize = 50;
    for (int i = 0; i < COL; ++i) {
        ui->tableWidget->setColumnWidth(i, buttonSize);
    }
    for (int i = 0; i < ROW; ++i) {
        ui->tableWidget->setRowHeight(i, buttonSize);
    }

    for (int i = 0; i < ROW; ++i) {
        for (int j = 0; j < COL; ++j) {
            QPushButton *button = new QPushButton();
            button->setFixedSize(buttonSize, buttonSize);
            ui->tableWidget->setCellWidget(i, j, button);

            connect(button, &QPushButton::clicked, this, [=]() {
                on_CellButton_clicked(i, j);
            });
        }
    }
}

void MainWindow::on_CellButton_clicked(int n0, int n1)
{
    win = true;
    QMessageBox msgBox;
    msgBox.setText("Выберите действие:");
    QPushButton *digButton = msgBox.addButton("Вскопать", QMessageBox::ActionRole);
    QPushButton *flagButton = msgBox.addButton("Флажок", QMessageBox::ActionRole);
    msgBox.exec();

    if (msgBox.clickedButton() == digButton) {
        if (start) {
            GenerateMatrix(n0, n1, ROW, COL, dynamicArray, mines);
            start = false;
        }
        if (dynamicArray[n0][n1] >= 0){
            ui->label_2->setText("Вы наткнулись на мину! Игра окончена.");
            BlockButtons();
            return;
        }
        else{
            Func(n0, n1, ROW, COL, dynamicArray);
        }
    } else if (msgBox.clickedButton() == flagButton) {
        QPushButton *button = qobject_cast<QPushButton*>(ui->tableWidget->cellWidget(n0, n1));
        if (button) {
            button->setText("⚑");
        }
    }
    for (int i = 0; i < ROW; ++i) {
        for (int j = 0; j < COL; ++j) {
            QPushButton *button = qobject_cast<QPushButton*>(ui->tableWidget->cellWidget(i, j));
            if (button && button->text().isEmpty()) {
                win = false;
                break;
            }
        }
        if (!win) break;
    }

    if (win) {
        ui->label_2->setText("ПОБЕДА!");
        BlockButtons();
        saveElapsedTime();
        return;
    }
}

void MainWindow::GenerateMatrix(int n0, int n1, int ROW, int COL, int **arr, int mines)
{
    bool del = true;
    arr[n0][n1] = -1;
    for (int i = 0; i < ROW; i++)
    {
        for (int j = 0; j < COL; j++)
        {
            if ((i == n0) && (j == n1)) {
                continue;
            }
            arr[i][j] = rand() % (ROW * COL) - ((ROW * COL) - mines);
            for (int k = 0; k < ROW; k++)
            {
                for (int g = 0; g < COL; g++)
                {
                    if ((i == k) && (j == g))
                    {
                        del = false;
                        break;
                    }
                    else if ((i == k) && (j + 1 == g))
                    {
                        del = false;
                        break;
                    }
                    else
                    {
                        if (arr[i][j] == arr[k][g])
                        {
                            del = false;
                            if (j == 0)
                            {
                                j = COL - 1;
                                i = i - 1;
                            }
                            else
                            {
                                j = j - 1;
                            }
                            break;
                        }
                    }
                }

                if (!del)
                {
                    del = true;
                    break;
                }
            }
        }
    }
}

void MainWindow::Func(int n0, int n1, int ROW, int COL, int **arr){
    int minecounter = 0;
    for (int i = n0-1; i<n0+2; i++){
        for (int j = n1-1; j<n1+2; j++){
            if ((i> -1) &&  (i < ROW) &&  (j > -1) && (j < COL)){
                QPushButton *button = qobject_cast<QPushButton*>(ui->tableWidget->cellWidget(i, j));
                if (button && (button->text() == "" || button->text() == "⚑")) {
                    if (arr[i][j] >= 0) {
                        minecounter++;
                    }
                }
            }
        }
    }
    QPushButton *currentButton = qobject_cast<QPushButton*>(ui->tableWidget->cellWidget(n0, n1));
        if (currentButton) {
            currentButton->setText(QString::number(minecounter));
        }

    if (minecounter == 0){
        for (int i = n0-1; i<n0+2; i++){
            for (int j = n1-1; j<n1+2; j++){
                if ((i> -1) &&  (i < ROW) &&  (j > -1) && (j < COL)){
                    QPushButton *button = qobject_cast<QPushButton*>(ui->tableWidget->cellWidget(i, j));
                    if (button && (button->text() == "" || button->text() == "⚑")) {
                         Func(i, j, ROW, COL, arr);
                    }
                }
            }
        }
    }
    else{
        return;
    }
}

void MainWindow::BlockButtons() {
    for (int i = 0; i < ROW; ++i) {
        for (int j = 0; j < COL; ++j) {
            QPushButton *button = qobject_cast<QPushButton*>(ui->tableWidget->cellWidget(i, j));
            if (button) {
                button->setEnabled(false);
            }
        }
    }
    timer->stop();
}

void MainWindow::updateTimer() {
    elapsedTime++;
    ui->label_time->setText(QString::number(elapsedTime));
}

void MainWindow::on_SaveButton_clicked()
{
    if (ui->tableWidget->rowCount() == 0) {
        QMessageBox::warning(this, "Ошибка", "Нет созданных строк в таблице. Пожалуйста, добавьте строки.");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить файл", "", "JSON Files (*.json);;All Files (*)");

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл для записи.");
        return;
    }

    QJsonObject saveData;
    saveData["rows"] = ROW;
    saveData["columns"] = COL;
    saveData["elapsedTime"] = elapsedTime;
    saveData["mines"] = mines;
    QJsonArray array;
    for (int i = 0; i < ROW; ++i) {
        QJsonArray rowArray;
        for (int j = 0; j < COL; ++j) {
            rowArray.append(dynamicArray[i][j]);
        }
        array.append(rowArray);
    }
    saveData["gameState"] = array;

    QJsonArray buttonStates;
    for (int i = 0; i < ROW; ++i) {
        QJsonArray buttonRow;
        for (int j = 0; j < COL; ++j) {
            QPushButton *button = qobject_cast<QPushButton*>(ui->tableWidget->cellWidget(i, j));
            if (button) {
                buttonRow.append(button->text());
            }
        }
        buttonStates.append(buttonRow);
    }
    saveData["buttonStates"] = buttonStates;

    QJsonDocument doc(saveData);
    file.write(doc.toJson());
    file.close();

    QMessageBox::information(this, "Успех", "Данные успешно сохранены в файл.");
}



void MainWindow::on_LoadButton_clicked()
{
    timer->start(1000);
    start = false;

    QString fileName = QFileDialog::getOpenFileName(this, "Загрузить файл", "", "JSON Files (*.json);;All Files (*)");

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл для чтения.");
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);

    if (doc.isNull() || !doc.isObject()) {
        QMessageBox::warning(this, "Ошибка", "Некорректный формат файла.");
        return;
    }

    QJsonObject loadData = doc.object();
    ROW = loadData["rows"].toInt();
    COL = loadData["columns"].toInt();
    elapsedTime = loadData["elapsedTime"].toInt();
    mines = loadData["mines"].toInt();
    ui->label_2->setText("");
    ui->label_time->setText(QString::number(elapsedTime));

    if (dynamicArray != nullptr) {
        for (int i = 0; i < ROW; ++i) {
            delete[] dynamicArray[i];
        }
        delete[] dynamicArray;
    }

    dynamicArray = new int*[ROW];
    for (int i = 0; i < ROW; ++i) {
        dynamicArray[i] = new int[COL]();
    }

    QJsonArray gameState = loadData["gameState"].toArray();
    for (int i = 0; i < ROW; ++i) {
        QJsonArray rowArray = gameState[i].toArray();
        for (int j = 0; j < COL; ++j) {
            dynamicArray[i][j] = rowArray[j].toInt();
        }
    }


    ui->tableWidget->horizontalHeader()->setVisible(false);
    ui->tableWidget->verticalHeader()->setVisible(false);

    ui->tableWidget->setRowCount(ROW);
    ui->tableWidget->setColumnCount(COL);

    ui->tableWidget->clear();

    int buttonSize = 50;
    for (int i = 0; i < COL; ++i) {
        ui->tableWidget->setColumnWidth(i, buttonSize);
    }
    for (int i = 0; i < ROW; ++i) {
        ui->tableWidget->setRowHeight(i, buttonSize);
    }

    for (int i = 0; i < ROW; ++i) {
        for (int j = 0; j < COL; ++j) {
            QPushButton *button = new QPushButton();
            button->setFixedSize(buttonSize, buttonSize);
            ui->tableWidget->setCellWidget(i, j, button);

            connect(button, &QPushButton::clicked, this, [=]() {
                on_CellButton_clicked(i, j);
            });
        }
    }

    QJsonArray buttonStates = loadData["buttonStates"].toArray();
    for (int i = 0; i < ROW; ++i) {
        QJsonArray buttonRow = buttonStates[i].toArray();
        for (int j = 0; j < COL; ++j) {
            QPushButton *button = qobject_cast<QPushButton*>(ui->tableWidget->cellWidget(i, j));
            if (button) {
                button->setText(buttonRow[j].toString());
            }
        }
    }
    file.close();
    QMessageBox::information(this, "Успех", "Данные успешно загружены из файла.");
}



void MainWindow::on_RatingButton_clicked() {
    QFile file("rating.json");
    if (!file.open(QIODevice::ReadOnly)) {
        // Если файл не существует, создаем его с пустыми массивами
        QJsonObject ratingObject;
        ratingObject["Лёгкая"] = QJsonArray();
        ratingObject["Средняя"] = QJsonArray();
        ratingObject["Сложная"] = QJsonArray();

        QJsonDocument doc(ratingObject);
        file.close();
        file.open(QIODevice::WriteOnly);
        file.write(doc.toJson());
        file.close();
        QMessageBox::information(this, "Рейтинг", "Рейтинг создан. Попробуйте сыграть и сохранить время.");
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull() || !doc.isObject()) {
        QMessageBox::warning(this, "Ошибка", "Некорректный формат файла рейтинга.");
        return;
    }

    QJsonObject ratingsObject = doc.object();
    QJsonArray easyRatings = ratingsObject["Лёгкая"].toArray();
    QJsonArray mediumRatings = ratingsObject["Средняя"].toArray();
    QJsonArray hardRatings = ratingsObject["Сложная"].toArray();

    QString ratingsText;
    auto appendRatings = [&ratingsText](const QString& title, const QJsonArray& ratings) {
        ratingsText += title + ":\n";
        for (const auto& rating : ratings) {
            ratingsText += QString("%1 секунд\n").arg(rating.toInt());
        }
        ratingsText += "\n";
    };

    appendRatings("Лёгкая", easyRatings);
    appendRatings("Средняя", mediumRatings);
    appendRatings("Сложная", hardRatings);

    QMessageBox::information(this, "Рейтинг", ratingsText);
}

void MainWindow::saveElapsedTime() {
    QFile file("rating.json");
    if (!file.open(QIODevice::ReadOnly)) {
        // Если файл не существует, создаем его с пустыми массивами
        QJsonObject ratingObject;
        ratingObject["Лёгкая"] = QJsonArray();
        ratingObject["Средняя"] = QJsonArray();
        ratingObject["Сложная"] = QJsonArray();

        QJsonDocument doc(ratingObject);
        file.close();
        file.open(QIODevice::WriteOnly);
        file.write(doc.toJson());
        file.close();
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject ratingsObject = doc.object();

    QString difficulty;
    int difficultyIndex = ui->DifficultyComboBox->currentIndex();
    if (difficultyIndex == 0) {
        difficulty = "Лёгкая";
    } else if (difficultyIndex == 1) {
        difficulty = "Средняя";
    } else {
        difficulty = "Сложная";
    }

    QJsonArray ratingsArray = ratingsObject[difficulty].toArray();
    ratingsArray.append(elapsedTime);

    // Сортировка массива вручную
    std::vector<int> tempArray;
    for (const auto& value : ratingsArray) {
        tempArray.push_back(value.toInt());
    }
    std::sort(tempArray.begin(), tempArray.end());

    // Обновляем ratingsArray с отсортированными значениями
    ratingsArray = QJsonArray();
    for (const auto& time : tempArray) {
        ratingsArray.append(time);
    }

    ratingsObject[difficulty] = ratingsArray;

    file.close();
    file.open(QIODevice::WriteOnly);
    QJsonDocument newDoc(ratingsObject);
    file.write(newDoc.toJson());
    file.close();
}
