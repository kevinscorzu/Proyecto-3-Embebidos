export class Square {
    public color = 'red';
    public x = 0;
    public y = 0;
    public w = 70;
    public h = 20;
    public id = -1;
    public on = false;
    public side = 'h';
  
    constructor(private ctx: CanvasRenderingContext2D) {}
  
    
  
     draw() {
      this.ctx.fillStyle = this.color;
      
      if (this.side == 'v'){
        this.h = 70;
        this.w = 20;
      }
      

      if (this.on==true){
        this.ctx.fillStyle = this.color;
        this.ctx.fillRect(this.x,this.y,this.w,this.h);
        } else {
            
            this.ctx.fillRect(this.x,this.y,this.w,this.h);
      }
    }
  }