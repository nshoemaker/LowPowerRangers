# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('testServer', '0002_auto_20160228_2345'),
    ]

    operations = [
        migrations.RemoveField(
            model_name='tag',
            name='id',
        ),
        migrations.AlterField(
            model_name='tag',
            name='distance',
            field=models.FloatField(null=True),
        ),
        migrations.AlterField(
            model_name='tag',
            name='tagID',
            field=models.IntegerField(serialize=False, primary_key=True),
        ),
        migrations.AlterField(
            model_name='tag',
            name='theta',
            field=models.FloatField(null=True),
        ),
    ]
