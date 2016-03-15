from django.conf.urls import include, url
from django.contrib import admin

from . import views

urlpatterns = [
	url(r'^set/(?P<tagID>[0-9]+)/(?P<B>[0-9]+\.?[0-9]*)/(?P<r1>[0-9]+\.?[0-9]*)/(?P<r2>[0-9]+\.?[0-9]*)', views.setData),
	url(r'^get/(?P<tagID>[0-9]+)/', views.getDataFromTagID),
]