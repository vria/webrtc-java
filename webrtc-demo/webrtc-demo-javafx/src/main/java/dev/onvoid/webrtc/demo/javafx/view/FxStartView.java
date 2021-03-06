/*
 * Copyright 2019 Alex Andres
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package dev.onvoid.webrtc.demo.javafx.view;

import dev.onvoid.webrtc.demo.javafx.util.FxUtils;
import dev.onvoid.webrtc.demo.view.Action;
import dev.onvoid.webrtc.demo.view.StartView;

import javafx.beans.InvalidationListener;
import javafx.beans.Observable;
import javafx.beans.binding.Bindings;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.layout.VBox;

@FxmlView(name = "start")
public class FxStartView extends VBox implements StartView {

	@FXML
	private TextField roomNameField;

	@FXML
	private Button enterButton;

	@FXML
	private Label errorLabel;


	@Override
	public String getRoomName() {
		return roomNameField.getText();
	}

	@Override
	public void setRoomName(String name) {
		FxUtils.invoke(() -> {
			roomNameField.setText(name);
		});
	}

	@Override
	public void setError(String message) {
		FxUtils.invoke(() -> {
			errorLabel.setText(message);
		});
	}

	@Override
	public void setOnConnect(Action action) {
		FxUtils.bindAction(enterButton, action);
	}

	@FXML
	private void initialize() {
		sceneProperty().addListener(new InvalidationListener() {

			@Override
			public void invalidated(Observable observable) {
				sceneProperty().removeListener(this);

				enterButton.requestFocus();
			}
		});

		enterButton.disableProperty().bind(Bindings.length(
				roomNameField.textProperty()).greaterThan(4).not());
	}

}
