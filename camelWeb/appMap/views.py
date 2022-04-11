from django.shortcuts import render
from django.http import JsonResponse

from libs import unixClient

# Create your views here.
def htmlHomeMap(request):
    return render(request, 'appMap/index.html')

def apiQuery(request):
    if request.method == 'GET':
        try:
            source_lng = float(request.GET.get('slng', ''))
            source_lat = float(request.GET.get('slat', ''))
            target_lng = float(request.GET.get('tlng', ''))
            target_lat = float(request.GET.get('tlat', ''))
            response_req = int(request.GET.get('response_req', ''))
            algorithm_req = int(request.GET.get('algorithm_req', ''))

            print(f"{source_lng} . {source_lat} . {target_lng} . {target_lat} . {response_req} . {algorithm_req}")

            request_obj = unixClient.UnixClient(source_lng, source_lat, target_lng, target_lat, response_req, algorithm_req)
            return JsonResponse(request_obj.DoRequest())

        except:
            return JsonResponse(unixClient.QueryResponse(1, 'required data is emptied', '{}').dump())
    else:
        return JsonResponse(unixClient.QueryResponse(1, 'method invalid', '{}').dump())
