<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.10.2" name="ragdoll_elements" tilewidth="2501" tileheight="1080" tilecount="10" columns="0" objectalignment="bottomleft">
 <editorsettings>
  <export target="ragdoll_elements..tsx" format="tsx"/>
 </editorsettings>
 <grid orientation="orthogonal" width="1" height="1"/>
 <tile id="0" type="crate">
  <properties>
   <property name="properties" type="class"/>
  </properties>
  <image width="64" height="64" source="crate01.png"/>
  <objectgroup draworder="index" id="2">
   <object id="20" x="0" y="0" width="64" height="64"/>
  </objectgroup>
 </tile>
 <tile id="5" type="crate">
  <properties>
   <property name="properties" type="class"/>
  </properties>
  <image width="64" height="64" source="crate02.png"/>
 </tile>
 <tile id="6" type="rocket">
  <properties>
   <property name="properties" type="class"/>
  </properties>
  <image width="50" height="57" source="rocket.png"/>
 </tile>
 <tile id="7" type="exit">
  <properties>
   <property name="properties" type="class"/>
  </properties>
  <image width="96" height="96" source="goaldoor.png"/>
 </tile>
 <tile id="8">
  <properties>
   <property name="properties" type="class"/>
  </properties>
  <image width="64" height="64" source="earthtile.png"/>
 </tile>
 <tile id="9">
  <image width="700" height="700" source="../textures/popart_goal.png"/>
  <objectgroup draworder="index" id="2">
   <object id="1" x="183.835" y="84.8352" width="351.33" height="542.33"/>
  </objectgroup>
 </tile>
 <tile id="10">
  <image width="1500" height="700" source="../textures/popart_platformbubble_right.png"/>
  <objectgroup draworder="index" id="2">
   <object id="1" x="40" y="97" width="1429" height="573">
    <properties>
     <property name="properties" type="class" propertytype="WallProperties">
      <properties>
       <property name="obstacle" type="class" propertytype="Obstacle">
        <properties>
         <property name="bodytype" type="int" propertytype="bodytype" value="0"/>
         <property name="damping" type="float" value="0"/>
         <property name="debugcolor" type="class" propertytype="Obstacle"/>
         <property name="debugopacity" type="float" value="192"/>
         <property name="density" type="float" value="0"/>
         <property name="friction" type="float" value="0"/>
         <property name="restitution" type="float" value="0"/>
         <property name="rotation" type="bool" value="false"/>
         <property name="texture" value="earth"/>
        </properties>
       </property>
      </properties>
     </property>
    </properties>
    <polygon points="0,0 1429,0 1429,573 0,573"/>
   </object>
  </objectgroup>
 </tile>
 <tile id="11">
  <image width="1500" height="700" source="../textures/popart_platformbubble_left.png"/>
  <objectgroup draworder="index" id="2">
   <object id="1" type="wall" x="102.399" y="76.0679" width="1252.19" height="596.84">
    <properties>
     <property name="properties" type="class" propertytype="WallProperties">
      <properties>
       <property name="obstacle" type="class" propertytype="Obstacle">
        <properties>
         <property name="bodytype" type="int" propertytype="bodytype" value="0"/>
         <property name="damping" type="float" value="0"/>
         <property name="debugcolor" type="class" propertytype="Obstacle"/>
         <property name="debugopacity" type="float" value="192"/>
         <property name="density" type="float" value="0"/>
         <property name="friction" type="float" value="0"/>
         <property name="restitution" type="float" value="0"/>
         <property name="rotation" type="bool" value="false"/>
         <property name="texture" value="popart_platformbubble_left"/>
        </properties>
       </property>
      </properties>
     </property>
    </properties>
    <polygon points="0,0 1252.19,0 1252.19,596.84 0,596.84"/>
   </object>
  </objectgroup>
 </tile>
 <tile id="12">
  <image width="2501" height="301" source="../textures/popart_platformLine.png"/>
 </tile>
 <tile id="13">
  <image width="2340" height="1080" source="../textures/popart_background.png"/>
 </tile>
</tileset>
