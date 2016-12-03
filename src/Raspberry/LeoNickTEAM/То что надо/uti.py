# coding: utf-8
from peewee import *

db = SqliteDatabase('places.db')


class Place(Model):
    identifier = IntegerField(unique=True)
    status = IntegerField()

    class Meta:
        database = db


def update_status(identifier, stat):
    identifier += 1
    if 1 << identifier << 16:
        query = Place.update(status=stat).where(Place.identifier == identifier)
        query.execute()
    else:
        return False
    return True


def get_status(identifier):
    identifier += 1
    if 1 << identifier << 16:
        query = Place.select().where(Place.identifier == identifier).get()
        return query.status
    else:
        return None
