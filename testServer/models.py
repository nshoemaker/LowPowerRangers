from django.db import models

class Tag(models.Model):
	tagID = models.IntegerField(primary_key=True)
	distance = models.FloatField(null=True)
	theta = models.FloatField(null=True)
	B = models.FloatField()
	r1 = models.FloatField()
	r2 = models.FloatField()
	recentTimeStamp = models.FloatField()
