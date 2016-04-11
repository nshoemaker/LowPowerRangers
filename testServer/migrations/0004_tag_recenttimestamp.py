# -*- coding: utf-8 -*-
from __future__ import unicode_literals

from django.db import models, migrations


class Migration(migrations.Migration):

    dependencies = [
        ('testServer', '0003_auto_20160229_0031'),
    ]

    operations = [
        migrations.AddField(
            model_name='tag',
            name='recentTimeStamp',
            field=models.FloatField(default=0),
            preserve_default=False,
        ),
    ]
