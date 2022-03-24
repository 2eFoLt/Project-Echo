#include "msqldatabase.h"
//!
//! \brief Конструктор объекта класса SQLdb
//!
SQLdb::SQLdb()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("client-db");
    if(!db.open())
        qDebug()<<db.lastError().text();
    query = QSqlQuery(db);//создали таблицу
    query.exec("CREATE TABLE User("
               "login VARCHAR(20) NOT NULL, "
               "password VARCHAR(20) NOT NULL"
               ")");
}

//!
//! \brief Деструктор объекта класса SQLdb
//!
SQLdb::~SQLdb()
{
    qDebug() << "destructor for SQLdb called";
    query.exec("DROP TABLE User");
    db.close();
}

//!
//! \brief Функция для добавления новой записи в БД
//! \param log Логин пользователя
//! \param pssw Пароль пользователя
//! \return Если пара найдена в базе, то регистрация считается успешной. Если в БД найден только логин, а пароль не совпадает - регистрация провалена.
//!
QString SQLdb::insert_new(QString log, QString pssw)
{
    pssw = pssw.remove((pssw.size()-2), (pssw.size()-1));
    qDebug() << log << pssw;
    query.prepare("SELECT login FROM User WHERE login==:login");
    query.bindValue(":login", log);
    query.exec();
    QSqlRecord rec = query.record();
    const int loginIndex = rec.indexOf("login");//номер "столбца"
    const int passwordIndex = rec.indexOf("password");
    qDebug() << loginIndex << passwordIndex << query.size();

    if(query.value(loginIndex).isNull())
    {
        query.prepare("INSERT INTO User(login, password) "
                          "VALUES (:login, :password)");
        query.bindValue(":password", pssw); query.bindValue(":login", log);
        query.exec();// выполнить запрос
        qDebug() << loginIndex << passwordIndex << query.value(loginIndex) << query.value(passwordIndex);
        return "register-success";
    }
    else if(query.value(log) != pssw) return "wrong-password";
    else return "user-already-exist";
}

QString SQLdb::auth(QString log, QString pssw)
{
    pssw = pssw.remove((pssw.size()-2), (pssw.size()-1));
    qDebug() << log << pssw;
    query.exec("SELECT * FROM User WHERE login==" + log);
    QSqlRecord rec = query.record();
    if(!rec.isEmpty())
    {
        if(query.value(log) != pssw) return "wrong-password";
        else return "auth-success";
    }
    else return "no-such-user";
}

//!
//! \brief Функция вывода всех записей БД
//!
void SQLdb::print_db()
{
    query.exec("SELECT * FROM User");
    QSqlRecord rec = query.record();
    const int loginIndex = rec.indexOf("login");
    const int passwordIndex = rec.indexOf("password");
    while(query.next())
        qDebug() << query.value(loginIndex).toString()
         << "\t" << query.value(passwordIndex) << "\n";
}