import argparse
import torch
from torchvision import transforms
from PIL import Image

from model import get_model

CLASSES = ["animals", "humans"]


def get_transform():
    return transforms.Compose(
        [
            transforms.Resize((224, 224)),
            transforms.ToTensor(),
            transforms.Normalize(mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225]),
        ]
    )


def predict(image_path, weights_path):
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

    model = get_model()
    model.load_state_dict(torch.load(weights_path, map_location=device))
    model.to(device)
    model.eval()

    transform = get_transform()

    image = Image.open(image_path).convert("RGB")
    tensor = transform(image).unsqueeze(0).to(device)

    with torch.no_grad():
        outputs = model(tensor)
        probs = torch.softmax(outputs, dim=1)[0]
        predicted_idx = probs.argmax().item()
        confidence = probs[predicted_idx].item() * 100

    label = CLASSES[predicted_idx]
    print(f"Result: {label} ({confidence:.2f}%)")
    return label


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Classify image: human or animal")
    parser.add_argument("image_path", type=str, help="Path to the image to classify")
    parser.add_argument(
        "--weights", type=str, default="weights.pth", help="Path to model weights file"
    )
    args = parser.parse_args()

    predict(args.image_path, args.weights)
