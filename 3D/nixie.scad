$fn=100;
addition= 0.3;

module ausschnitt()
{  
  rotate([90,0,0])translate([0,0,-1.5-addition])
  linear_extrude(height=3+2*addition)
    polygon(points=[[-2.5-addition,-1],[-2.5-addition,5-1-addition], [-2.5-1-addition,5+addition]
      , [2.5+1+addition,5+addition],[2.5+addition,5-1-addition],[2.5+addition,-1]
  
  ]);
}

module auschnittDigit()
{
  for(i=[0:4],n=[0:1])
  {
    translate([9+n*20.666,7+i*6,0]) ausschnitt();
    translate([9+10.333+n*20.666,10+i*6,0]) ausschnitt();
  }
  hull()
  {
    translate([6.5,7-1.5-addition,8])rotate([-90,0,0])cylinder(r=3+addition,h=30+2*addition);
    translate([42+.5,7-1.5-addition,8])rotate([-90,0,0])cylinder(r=3+addition,h=30+2*addition);
  }
  translate([3.5-addition/2,7-1.5-addition,8]) cube([42+addition,30+2*addition,20]);
}

module miniBoden()
{
  difference()
  {
    union()
    {
      cube([25,40,8]);
      
      difference()
      {
        translate([0,16.7-1.5-addition-2,8]) cube([25,12+(2+addition)*2,10]);
        
        translate([3.2+2,16.7-5,8]) cube([18.6-4,20,15]);
      }
    }
        
    translate([9,0,0]) 
    {
      translate([0,16.7,0]) ausschnitt();
      translate([7,16.7+3,0]) ausschnitt();
      translate([0,16.7+6,0]) ausschnitt();
      translate([7,16.7+9,0]) ausschnitt();
    }  
    
    hull()
    {
      translate([6.5,16.7-1.5-addition,8])rotate([-90,0,0])cylinder(r=3+addition,h=12+2*addition);
      translate([18+.5,16.7-1.5-addition,8])rotate([-90,0,0])cylinder(r=3+addition,h=12+2*addition);
    }
    
    translate([3.5-addition,16.7-1.5-addition,8]) cube([18+2*addition,12+2*addition,20]);
    
    translate([ 3.5, 3.5,-1]) cylinder(d=2.5,h=7);
    translate([ 3.5,36.5,-1]) cylinder(d=2.5,h=7);
    translate([21.5, 3.5,-1]) cylinder(d=2.5,h=7);
    translate([21.5,36.5,-1]) cylinder(d=2.5,h=7);

  }
}

module boden()
{
  difference()
  {
    union()
    {
      cube([99,40,8]);
      
      difference()
      {
        translate([0,0,8]) cube([99,40,10]);
        
        translate([3.2+2,-1,8]) cube([42+2*addition-4,50,15]);
        translate([48+3.2+2,-1,8]) cube([42+2*addition-4,50,15]);
      }
    }
    
    auschnittDigit();
    translate([48,0,0]) auschnittDigit();
    
    translate([ 3.5, 3.5,-1]) cylinder(d=2.5,h=7);
    translate([ 3.5,36.5,-1]) cylinder(d=2.5,h=6);
    translate([49.5, 3.5,-1]) cylinder(d=2.5,h=7);
    translate([49.5,36.5,-1]) cylinder(d=2.5,h=7);
    translate([95.5, 3.5,-1]) cylinder(d=2.5,h=7);
    translate([95.5,36.5,-1]) cylinder(d=2.5,h=7);
  }
}

module schraubAussparung()
{
  cylinder(d=3.5, h=10);
  cylinder(d=6.5, h=4.5);
}

module vorne()
{
  translate([0,-4.5,-5-3.75])
  {
    translate([0,0,-10]) cube([233,3.5,13]);
    *translate([0,0,-10]) cube([233,4,3]);
    difference()
    {
      translate([0,0,1.5]) cube([233,12,13]);
      translate([-1,3.5,5]) cube([240,20,20]);
      
      translate([ 3.5,8,-3]) schraubAussparung();
      translate([49.5,8,-3]) schraubAussparung();
      translate([95.5,8,-3]) schraubAussparung();
      
      translate([107.5,8,-3]) schraubAussparung();
      translate([107.5+18,8,-3]) schraubAussparung();
      
      translate([134,0,0])
      {
        translate([ 3.5,8,-3]) schraubAussparung();
        translate([49.5,8,-3]) schraubAussparung();
        translate([95.5,8,-3]) schraubAussparung();
      }
    }
  }
}

vorne();
translate([233,40,0]) rotate([0,0,180]) vorne();

boden();
translate([104,0,0]) miniBoden();
translate([134,0,0]) boden();
