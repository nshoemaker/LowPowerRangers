# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('testServer', '0001_initial'),
    ]

    operations = [
        migrations.CreateModel(
            name='Tag',
            fields=[
                ('id', models.AutoField(verbose_name='ID', serialize=False, auto_created=True, primary_key=True)),
                ('tagID', models.IntegerField()),
                ('distance', models.FloatField()),
                ('theta', models.FloatField()),
                ('B', models.FloatField()),
                ('r1', models.FloatField()),
                ('r2', models.FloatField()),
            ],
        ),
        migrations.DeleteModel(
            name='Message',
        ),
    ]
