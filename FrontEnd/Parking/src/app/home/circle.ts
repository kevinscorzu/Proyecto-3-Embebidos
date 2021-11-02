export class Circle {
    public color = 'red';
    public x = 0;
    public y = 0;
    public r = 20;
    public on = false;
    public id = -1;
    public firstTime = true;

  
    constructor(private ctx: CanvasRenderingContext2D) {}
  
    
  
     draw() {
      this.ctx.beginPath();
      this.ctx.fillStyle =  this.color;
      this.ctx.fill();
      this.ctx.arc(this.x,this.y,this.r,0,2*Math.PI);
      if (this.firstTime){
        this.ctx.stroke()
        return;
      }
      else if (this.on==true){
        this.ctx.fillStyle =  'green';
        this.ctx.fill();
        return;
      }
      
      else {
        this.ctx.fillStyle =  'red';
        this.ctx.fill();
      }
      }
  }