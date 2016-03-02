from django.shortcuts import render
import math
from django.http import JsonResponse, HttpResponse
from testServer.models import *

def calcDistanceAndTheta(B, r1, r2, old_distance, old_theta):
	try:
		distance = math.sqrt(0.5 * (r1**2) - 0.25 * (B**2) + 0.5 * (r2**2))
		theta = (math.acos((distance ** 2 + (B/2)**2 - r1**2) / (distance * B)) - math.pi / 2.0) * (180 / math.pi)
		return (distance, theta)
	except ValueError:
		return (old_distance, old_theta)


def setData(request, tagID, B, r1, r2):
	tagID = int(tagID)
	B = float(B)
	r1 = float(r1)
	r2 = float(r2)
	currTag = Tag.objects.all().filter(tagID=tagID).first()
	if(currTag != None):
		currTag.B = B
		currTag.r1 = r1
		currTag.r2 = r2
	else:
		currTag = Tag(tagID=tagID, B=B, r1=r1, r2=r2, distance=None, theta=None )
	(dist, theta) = calcDistanceAndTheta(B, r1, r2, currTag.distance, currTag.theta)

	currTag.distance = dist
	currTag.theta = theta
	currTag.save()

	if(dist != None and theta!= None):
		html = "<html><body>It has been set to R=%f and theta=%f" % (dist , theta)
	else:
		html = "<html><body>Distance and theta are None as an error occured"
	return HttpResponse(html)

def getDataFromTagID(request, tagID):
	response = {}
	currTag = Tag.objects.all().filter(tagID=tagID).first()
	if (currTag != None):
		response['R'] = currTag.distance
		response['theta'] = currTag.theta

		return JsonResponse(response)
	else:
		return JsonResponse({'error' : 'Couldnt find object with that ID'})

