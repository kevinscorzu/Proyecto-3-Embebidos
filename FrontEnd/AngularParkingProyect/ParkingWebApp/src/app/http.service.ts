import { Injectable } from '@angular/core';
import { HttpClient, HttpHeaders } from '@angular/common/http';

@Injectable({
  providedIn: 'root'
})
export class HttpService {

  constructor(private http: HttpClient) {
  }
  server = 'http://8b93-186-176-104-13.ngrok.io';

  getStates() {
    return this.http.get(this.server + '/Api/ParkingState');
  }

}
