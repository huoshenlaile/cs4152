<?xml version="1.0" encoding="UTF-8"?>
<tileset version="1.10" tiledversion="1.10.2" name="ragdoll_elements" tilewidth="214" tileheight="100" tilecount="6" columns="0" objectalignment="bottomleft">
 <editorsettings>
  <export target="ragdoll_elements..tsx" format="tsx"/>
 </editorsettings>
 <grid orientation="orthogonal" width="1" height="1"/>
 <tile id="0" type="crate">
  <properties>
   <property name="properties" type="class" propertytype="CrateProperties">
    <properties>
     <property name="obstacle" type="class" propertytype="Obstacle">
      <properties>
       <property name="bodytype" type="int" propertytype="bodytype" value="2"/>
       <property name="damping" type="float" value="1"/>
       <property name="debugcolor" propertytype="debugcolor" value="yellow"/>
       <property name="debugopacity" type="float" value="192"/>
       <property name="density" type="float" value="1"/>
       <property name="friction" type="float" value="0.2"/>
       <property name="restitution" type="float" value="0.1"/>
       <property name="rotation" type="bool" value="false"/>
       <property name="texture" value="crate01"/>
      </properties>
     </property>
    </properties>
   </property>
  </properties>
  <image width="64" height="64" source="crate01.png"/>
  <objectgroup draworder="index" id="2">
   <object id="20" x="0" y="0" width="64" height="64"/>
  </objectgroup>
 </tile>
 <tile id="5" type="crate">
  <properties>
   <property name="properties" type="class" propertytype="CrateProperties">
    <properties>
     <property name="obstacle" type="class" propertytype="Obstacle">
      <properties>
       <property name="bodytype" type="int" propertytype="bodytype" value="2"/>
       <property name="damping" type="float" value="1"/>
       <property name="debugcolor" propertytype="debugcolor" value="yellow"/>
       <property name="debugopacity" type="float" value="192"/>
       <property name="density" type="float" value="1"/>
       <property name="friction" type="float" value="0.2"/>
       <property name="restitution" type="float" value="0.1"/>
       <property name="rotation" type="bool" value="false"/>
       <property name="texture" value="crate02"/>
      </properties>
     </property>
    </properties>
   </property>
  </properties>
  <image width="64" height="64" source="crate02.png"/>
 </tile>
 <tile id="6" type="rocket">
  <properties>
   <property name="properties" type="class" propertytype="RocketProperties">
    <properties>
     <property name="burn" value="burn"/>
     <property name="flames" value="flames"/>
     <property name="flames-left" value="flames-left"/>
     <property name="flames-right" value="flames-right"/>
     <property name="left-sound" value="left-burn"/>
     <property name="main-sound" value="burn"/>
     <property name="obstacle" type="class" propertytype="Obstacle">
      <properties>
       <property name="bodytype" type="int" propertytype="bodytype" value="2"/>
       <property name="damping" type="float" value="0"/>
       <property name="debugcolor" propertytype="debugcolor" value="yellow"/>
       <property name="debugopacity" type="float" value="192"/>
       <property name="density" type="float" value="1"/>
       <property name="friction" type="float" value="0.1"/>
       <property name="restitution" type="float" value="0.4"/>
       <property name="rotation" type="bool" value="false"/>
       <property name="texture" value="rocket"/>
      </properties>
     </property>
     <property name="right-sound" value="right-burn"/>
     <property name="thrust" type="float" value="30"/>
    </properties>
   </property>
  </properties>
  <image width="50" height="57" source="rocket.png"/>
 </tile>
 <tile id="7" type="exit">
  <properties>
   <property name="properties" type="class" propertytype="ExitProperties">
    <properties>
     <property name="obstacle" type="class" propertytype="Obstacle">
      <properties>
       <property name="bodytype" type="int" propertytype="bodytype" value="0"/>
       <property name="damping" type="float" value="0"/>
       <property name="debugcolor" propertytype="debugcolor" value="yellow"/>
       <property name="debugopacity" type="float" value="192"/>
       <property name="density" type="float" value="0"/>
       <property name="friction" type="float" value="0"/>
       <property name="restitution" type="float" value="0"/>
       <property name="rotation" type="bool" value="false"/>
       <property name="texture" value="goal"/>
      </properties>
     </property>
    </properties>
   </property>
  </properties>
  <image width="96" height="96" source="goaldoor.png"/>
 </tile>
 <tile id="8">
  <properties>
   <property name="properties" type="class" propertytype="CrateProperties">
    <properties>
     <property name="obstacle" type="class" propertytype="Obstacle">
      <properties>
       <property name="bodytype" type="qlonglong" propertytype="bodytype" value="2"/>
       <property name="damping" type="float" value="1"/>
       <property name="debugcolor" propertytype="debugcolor" value="yellow"/>
       <property name="debugopacity" type="float" value="192"/>
       <property name="density" type="float" value="1"/>
       <property name="friction" type="float" value="0.2"/>
       <property name="restitution" type="float" value="0.1"/>
       <property name="rotation" type="bool" value="false"/>
       <property name="texture" value="crate01"/>
      </properties>
     </property>
    </properties>
   </property>
  </properties>
  <image width="64" height="64" source="earthtile.png"/>
 </tile>
 <tile id="9">
  <image width="214" height="100" source="../textures/popleft.png"/>
  <objectgroup draworder="index" id="2">
   <object id="1" x="19.5618" y="10.0604" width="175.497" height="85.5131"/>
  </objectgroup>
 </tile>
</tileset>
