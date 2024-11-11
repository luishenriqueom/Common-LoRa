import express, { Request, Response } from 'express';
import bodyParser from 'body-parser';
import { Sequelize, DataTypes, Model } from 'sequelize';

const app = express();
const PORT = 3000;

// Configuração do Sequelize para SQLite
const sequelize = new Sequelize({
    dialect: 'sqlite',
    storage: './sensor_data.db',
});

// Definição do modelo de dados
class SensorData extends Model {
    public id!: number;
    public device_id!: string;
    public temperature!: number;
    public humidity!: number;
    public luminosity!: number;
    public pdr!: number;
    public rssi!: number;
    public timestamp!: Date;
}

SensorData.init({
    device_id: {
        type: DataTypes.STRING,
        allowNull: false,
    },
    temperature: {
        type: DataTypes.FLOAT,
        allowNull: false,
    },
    humidity: {
        type: DataTypes.FLOAT,
        allowNull: false,
    },
    luminosity: {
        type: DataTypes.FLOAT,
        allowNull: false,
    },
    pdr: {
        type: DataTypes.FLOAT,
        allowNull: false,
    },
    rssi: {
        type: DataTypes.INTEGER,
        allowNull: false,
    },
    timestamp: {
        type: DataTypes.DATE,
        defaultValue: DataTypes.NOW,
    },
}, {
    sequelize,
    modelName: 'SensorData',
    timestamps: false,
});

app.use(bodyParser.json());

// Sincronizar com o banco de dados
sequelize.sync()
    .then(() => console.log("Banco de dados sincronizado"))
    .catch((err) => console.error("Erro ao sincronizar o banco de dados:", err));

// Endpoint para receber dados via POST
//@ts-ignore
app.post('/data', async (req: Request, res: Response) => {
    const { device_id, temperature, humidity, luminosity, pdr, rssi } = req.body;

    if (!device_id || temperature === undefined || humidity === undefined || luminosity === undefined || pdr === undefined || rssi === undefined) {
        return res.status(400).json({ error: 'Invalid data format' });
    }

    try {
        const newData = await SensorData.create({ device_id, temperature, humidity, luminosity, pdr, rssi });
        res.status(201).json({ message: 'Data received successfully', data: newData });
    } catch (error) {
        res.status(500).json({ error: 'Error saving data' });
    }
});

// Endpoint para recuperar dados via GET
app.get('/data', async (_req: Request, res: Response) => {
    try {
        const data = await SensorData.findAll();
        res.status(200).json(data);
    } catch (error) {
        res.status(500).json({ error: 'Error retrieving data' });
    }
});

app.listen(PORT, () => {
    console.log(`Servidor rodando na porta ${PORT}`);
});
