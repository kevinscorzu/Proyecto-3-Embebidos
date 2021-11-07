import { Component,ViewChild, ElementRef, OnInit ,NgZone } from '@angular/core';
import { HttpService } from './http.service';
import { Square } from './square';
import { Circle } from './circle';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css']
})
export class AppComponent implements OnInit {

  title = 'ParkingWebApp';

    @ViewChild('canvas', { static: true })
    canvas:ElementRef<HTMLCanvasElement>;
    
    private ctx: CanvasRenderingContext2D = null as any;
    doors: Square[] = [];
    lights: Circle[] = [];
    
    h = 100;
    w = 100;
    
    
    requestId:any;
    interval:any;
    
    
    ngOnInit(): void {
      }
    
      initializeLights(){
        var light0 = new Circle(this.ctx);
        light0.x = 50;
        light0.y = 100;
        light0.id = 4;
        light0.semaphore = 0;
        this.lights.push(light0)
    
        var light1 = new Circle(this.ctx);
        light1.x = 150;
        light1.y = 100;
        light1.id = 3;
        light1.semaphore = 0;
        this.lights.push(light1)
    
        var light2 = new Circle(this.ctx);
        light2.x = 250;
        light2.y = 100;
        light2.id = 2;
        light2.semaphore = 0;
        this.lights.push(light2)
    
        var light3 = new Circle(this.ctx);
        light3.x = 350;
        light3.y = 100;
        light3.id = 1;
        light3.semaphore = 0;
        this.lights.push(light3)
    
        var light4 = new Circle(this.ctx);
        light4.x = 450;
        light4.y = 100;
        light4.id = 5;
        light4.on = true;
        light4.semaphore = 1; 
        this.lights.push(light4)
    
        var light5 = new Circle(this.ctx);
        light5.x = 550;
        light5.y = 100;
        light5.id = 6;
        light5.on = true;
        light5.semaphore = 2; 
        this.lights.push(light5)
    
        for(let light of this.lights){
          light.draw()
          light.firstTime = false;
          }
      }
      reDrawLights(){
        for(let light of this.lights){
          light.draw()
          
          }
      }
      reDrawDoors(){
        for(let door of this.doors){
          door.draw()
          
          }
      }
      updateAll(){
        this.reDrawLights();
        this.reDrawDoors();
        
      }
      updateInfo(){
        this.http.getStates().subscribe((data) => {
          var json = JSON.parse(JSON.stringify(data));
          console.log(json);
          
          for (let element of this.lights){
            if (json["assignedSpace"] != 0){
              for (let el of this.lights){
                if (json["assignedSpace"] == el.id){
                  el.waiting = 1; 
                }
              }
            }
            if (json["assignedSpace"] == 0){
              for (let el of this.lights){
                el.waiting = 0;
              }
            }
            if (1 <= element.id && element.id <= 4){
              var status = json["space" + String(element.id)];
              if (status == 1){
                element.on = false;
              }
              else{
                element.on = true;
              }
            }
            else if (element.id == 5){
              var status = json["greenLed"];
              element.on = status;
            }
            else if (element.id == 6){
              var status = json["redLed"];
              element.on = status;
            }
            
          }
          
          this.reDrawLights();
          this.reDrawDoors();
        }, (error) => {
          console.log("Error al realizar la actualizacion de la casa");
          console.log(error);
        });
      }
      
    
    drawBackground(){
      
      this.ctx.strokeRect(0, 0, 100, 200); //Parqueo 1
      this.ctx.strokeRect(100, 0, 100, 200); //Parqueo 2
      this.ctx.strokeRect(200, 0, 100, 200); //Parqueo 3
      this.ctx.strokeRect(300, 0, 100, 200); //Parqueo 4
    
      this.ctx.font = 'italic 18px Arial';
      this.ctx.fillText('Parqueo 4', 10, 20);
      this.ctx.fillText('Parqueo 3', 110, 20);
      this.ctx.fillText('Parqueo 2', 210, 20);
      this.ctx.fillText('Parqueo 1', 310, 20);
      this.ctx.fillText('Semaforo', 460, 60);
    
      this.ctx.fillText('Entrada', 410, 220);
      this.ctx.fillText('Salida', 2, 290);
    
      //Dibujamos las lineas de salida y entrada
      this.ctx.fillRect(400,200,1,30);
      this.ctx.fillRect(400,270,1,30);
    
      this.ctx.fillRect(50,300,1,30);
    
      this.ctx.fillRect(400,200,200,1);
    
      this.ctx.fillRect(50,300,600,1);
      
    }
    ngAfterViewInit() {
      setTimeout(() => {
        var canvasEl: HTMLCanvasElement = this.canvas.nativeElement;
        var ctx = canvasEl.getContext('2d');
        this.ctx = ctx!;
        
        this.drawBackground();
        this.initializeLights();
        this.updateInfo();
        
      }, 1);
    }
    
    actionLight(id: number){
      for(let light of this.lights){
        if (light.id == id){
          light.on = !light.on;
        }
        }
        this.reDrawLights();
    }
    actionDoor(id: number){
      for(let door of this.doors){
        if (door.id == id){
          door.on = !door.on;
        }
        }
    }
    
    constructor(private http: HttpService) {

    }
    
    animate(): void {}
    
    }
