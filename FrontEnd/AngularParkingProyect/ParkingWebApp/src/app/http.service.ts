import { Injectable } from '@angular/core';
import { HttpClient, HttpHeaders } from '@angular/common/http';

@Injectable({
  providedIn: 'root'
})
export class HttpService {

  constructor(private http: HttpClient) {
  }
  server = 'http://192.168.1.101:9090';

  getStates() {
    return this.http.get(this.server + '/Api/ParkingState');
  }

}
