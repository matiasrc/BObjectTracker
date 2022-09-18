

#include "ofApp.h"

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.txt)
static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

//--------------------------------------------------------------
void ofApp::drawGui(){
    //required to call this at beginning
    gui.begin();
    // -------- MENU PRINCIPAL --------
    if (ImGui::BeginMainMenuBar())
    {
         
        if (ImGui::BeginMenu("|Face Tracker|"))
            {
                ImGui::Text("Preset: ");
                ImGui::RadioButton("Rápida", &preset, 0); ImGui::SameLine();
                ImGui::RadioButton("Accurate", &preset, 1); ImGui::SameLine();
                ImGui::RadioButton("Snsitiva", &preset, 2); ImGui::SameLine(); HelpMarker("elegir el preset para determinar la relación ebtre la rapidez y la precisión del algorítmo");
                  
                if(preset == 0 ){
                    finder.setPreset(ofxCv::ObjectFinder::Fast);
                }else if(preset == 1){
                    finder.setPreset(ofxCv::ObjectFinder::Accurate);
                }else if(preset == 2){
                    finder.setPreset(ofxCv::ObjectFinder::Sensitive);
                }
            
                     
                ImGui::Separator();
                ImGui::Text("OBJETO a detectar: ");
                if(ImGui::BeginCombo("", fileNames[fileID].c_str())){
        
                    for(int i=0; i < fileNames.size(); ++i){
                        const bool isSelected = (fileID == i);
                        if(ImGui::Selectable(fileNames[i].c_str(), isSelected)){
                            fileID = i;
                            cascadeFile = fileNames[i];
                            finder.setup(cascadeFile);
                        }
                        if(isSelected){
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                ImGui::EndCombo();
                }
                ImGui::SameLine(); HelpMarker("Elegir el archivo haar para detectar diferentes objetos (caras, ojos, boca, etc.)");
            
            ImGui::EndMenu();
            }
        if (ImGui::BeginMenu("|Entrada|"))
            {
                static const char* item_current = devicesVector[deviceID].c_str();
                if(ImGui::BeginCombo(" ", item_current)){
        
                    for(int i=0; i < devicesVector.size(); ++i){
                        const bool isSelected = (deviceID == i);
                        if(ImGui::Selectable(devicesVector[i].c_str(), isSelected)){
                            deviceID = i;
                            resetCameraSettings(deviceID);
                            item_current = devicesVector[i].c_str();
                        }
                        if(isSelected){
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                ImGui::EndCombo();
                }
                ImGui::SameLine(); HelpMarker("Elegir el dispositivo de entrada");
                
                ImGui::Separator();
                ImGui::Checkbox("ESPEJAR HORIZONTAL", &hMirror);
                ImGui::Checkbox("ESPEJAR VERTICAL", &vMirror);
                
            ImGui::EndMenu();
            }
        if (ImGui::BeginMenu("|OSC|"))
        {
            if(ImGui::InputInt("port", &puerto)) sender.setup(host, puerto);
            ImGui::SameLine(); HelpMarker("puerto de conexión");
            
            static char str1[128];
            strcpy(str1, host.c_str());
            //static char str1[128] = "127.0.0.1";
            //ImGui::InputTextWithHint("ip", "enter ip address here", str1, IM_ARRAYSIZE(str1));
            if( ImGui::InputTextWithHint("ip", "enter ip address here",str1, IM_ARRAYSIZE(str1))){
                host = ofVAArgsToString(str1);
                sender.setup(host, puerto);
                //ofLogVerbose() << "--------CAMBIO DE HOST: " << host;
            }
            ImGui::SameLine(); HelpMarker("dirección ip del receptor de mensajes");
            
            ImGui::Separator();
            
            static char datosaddress[128];
            strcpy(datosaddress, etiquetaMensajeDatos.c_str());
            if( ImGui::InputTextWithHint("address", "tipear etiqueta BLOBS",datosaddress, IM_ARRAYSIZE(datosaddress))){
                etiquetaMensajeDatos = ofVAArgsToString(datosaddress);
                //ofLogVerbose() << "--------CAMBIO DE ETIQUETA: " << movimientoaddress;
            }
            ImGui::SameLine(); HelpMarker("etiqueta (debe comenzar con /) ");
            
            ImGui::Checkbox("ENVIAR DATOS", &enviarDatos);
            ImGui::SameLine(); HelpMarker("habilitar / deshabilitar el envío de datos");
            
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("|Acerca|"))
        {
            ImGui::Text("BFaceTracker");
            ImGui::Separator();
            ImGui::Text("Software experimental para captura de movimiento.");
            ImGui::Text("utilizando las técnicas de haar cascade ");
            ImGui::Text("para detección de objetos (caras, ojos, nariz, etc.)");
            ImGui::Text("Esta aplicación está en desarrollo y no tiene soporte");
            ImGui::Text("..............");
            ImGui::Text("Desarrollado por Matías Romero Costas (Biopus)");
            ImGui::Text("www.biopus.ar");

            ImGui::EndMenu();
        }
        /*
        if (ImGui::BeginMenu("| Guardar", "cmd+s"))
        {
            saveSettings();
            ofLogVerbose() << "Configuración guardada";
            if (ImGui::MenuItem("Guardar", "cmd+s")) {
                saveSettings();
                ofLogVerbose() << "Configuración guardada";
            }
            
            ImGui::EndMenu();
        }
        */
        ImGui::EndMainMenuBar();
    }
    gui.end();
}
