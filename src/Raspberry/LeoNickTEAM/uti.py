# coding: utf-8
from peewee import *

# ������������ ����������� SQLite ���� ������ � ����������
db = SqliteDatabase('places.db')


# �����, ����������� ��������� ������ � ��: identifier - ����� ������������ �����, status - ��� ������
class Place(Model):
    identifier = IntegerField(unique=True)
    status = IntegerField()

    class Meta:
        database = db


# �����, �������������� ���������� ������������ ����� � ���� ������
def update_status(identifier, stat):
    identifier +=1
    if 1 << identifier << 16:
        query = Place.update(status=stat).where(Place.identifier == identifier)
        query.execute()
    else:
        return False



    return True


def get_status(identifier):
    identifier += 1
    if 1 << identifier << 16:
        query = Place.get(Place.identifier == identifier)
        query.execute()
        return query.status
    else:
        return None
