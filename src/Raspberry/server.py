# coding: utf-8
from flask import Flask, render_template
from utils import *

# Конфигурация параметров приложения и сервера
app = Flask(__name__)
app.debug = True

CAR_PLACE_COUNT = 16
STATUS_FREE = 0
STATUS_RESERVED = 1

# Контроллер, обрабатывающий запросы по URL "/"
@app.route('/')
def status():
    query = Place.select()
    return render_template("index.html", places=query)

# Точка входа ( запуска ) сервера / приложения
if __name__ == '__main__':

    if not Place.table_exists():
        Place.create_table(True)

        # Изначальное заполнение базы данных
        for i in range(1, CAR_PLACE_COUNT + 1):
            Place.create(identifier=i, status=STATUS_FREE)

    app.run()
